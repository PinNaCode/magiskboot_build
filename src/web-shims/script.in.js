if (typeof window !== 'undefined') {
    // window is present, we are running in a browser

    var Module = {
        'thisProgram': '@MAGISKBOOT_WASM_NAME@',
        'noInitialRun': true,  // prevent calling main() on page load
        'instantiateWasm': (imps, cb) => {
            var status_label = document.getElementById('status_label').childNodes[0];
            var status_show = document.getElementById('status_show');

            status_label.textContent = 'Fetching: ';
            status_show.textContent = '-- % (? / ?)';

            // fetch wasm

            const xhr = new XMLHttpRequest();

            xhr.open('GET', '@MAGISKBOOT_WASM_NAME@.wasm', true);
            xhr.responseType = 'arraybuffer';
            xhr.onprogress = (ev) => {
                if (ev.lengthComputable) {
                    const pct = Math.floor(100 * (ev.loaded / ev.total));
                    status_show.textContent = `${pct} % (${ev.loaded} / ${ev.total})`;
                } else
                    status_show.textContent = `-- % (${ev.loaded} / ?)`;
            };
            xhr.onload = async () => {
                var succeed = false;
                var wasm = null;

                if (xhr.status === 200) {
                    status_label.textContent = 'Compiling: ';
                    status_show.textContent = 'Take longer on slow devices';

                    try {
                        wasm = await WebAssembly.instantiate(xhr.response, imps);
                        succeed = true;
                    } catch (exc) {
                        status_label.textContent = 'WebAssembly Error: ';
                        status_show.textContent = exc.message;
                    }
                } else {
                    status_label.textContent = 'HTTP Error: ';
                    status_show.textContent = xhr.statusText;
                }

                if (succeed)
                    cb(wasm.instance);
                else
                    Module.abort();  // or emscripten will wait forever
            };
            xhr.onerror = (_) => {
                status_label.textContent = 'Network Error';
                status_show.textContent = '';

                Module.abort();  // or emscripten will wait forever
            };
            xhr.onabort = (_) => {
                status_label.textContent = 'Network Error: ';
                status_show.textContent = 'Connection is aborted';

                Module.abort();  // or emscripten will wait forever
            };
            xhr.ontimeout = (_) => {
                status_label.textContent = 'Network Error: ';
                status_show.textContent = 'Request timeout';

                Module.abort();  // or emscripten will wait forever
            };
            xhr.send();

            return {};
        },
        'preInit': () => {
            const scr_sel = document.getElementById('scr_sel');
            const scr_cmd = document.getElementById('scr_cmd');
            const scr_env = document.getElementById('scr_env');
            const scrs = [scr_cmd, scr_env];

            scr_sel.addEventListener('wheel', (ev) => {
                ev.preventDefault();

                // allow changing screen by scrolling

                if (ev.deltaY < 0)
                    scr_sel.selectedIndex = Math.max(scr_sel.selectedIndex - 1, 0);
                else
                    scr_sel.selectedIndex = Math.min(scr_sel.selectedIndex + 1, scr_sel.length - 1);

                scr_sel.dispatchEvent(new Event('change'));
            });

            scr_sel.addEventListener('change', () => {
                // switch screen

                var scr = null;

                switch (scr_sel.value) {
                    case 'cmd':
                        scr = scr_cmd;
                        break;
                    case 'env':
                        scr = scr_env;
                        break;
                }

                if (scr === null)
                    return;

                scrs.forEach((i) => {
                    var disp = null;

                    if (i === scr)
                        disp = 'flex';
                    else
                        disp = 'none';

                    i.style.display = disp;
                });
            });

            // bind stdout

            const conout = document.getElementById('conout');
            const _dec = new TextDecoder();
            var con_upd = null;

            Module.TTY.stream_ops.write = (_, buff, off, len) => {
                const arr = buff.subarray(off, off + len);
                conout.value += _dec.decode(arr);

                if (con_upd !== null)
                    clearTimeout(con_upd);
                con_upd = setTimeout(() => {
                    conout.scrollTop = conout.scrollHeight;
                }, 150);

                return len;
            }

            // exec ctrl

            const status_label = document.getElementById('status_label').childNodes[0];
            const status_show = document.getElementById('status_show');
            window.onerror = (_) => {
                status_show.textContent = 'Crashed';
            };

            const cmdline_edit = document.getElementById('cmdline_edit');
            var status_upd = null;
            Module.onRuntimeInitialized = () => {
                // set initial cwd to a nice place
                Module.FS.chdir('/home/web_user');

                // we can call main() now
                status_label.textContent = 'Status: ';
                status_show.textContent = 'Idle';
                cmdline_edit.readOnly = false;
                scr_sel.disabled = false;
            };
            cmdline_edit.addEventListener('keydown', (ev) => {
                if (ev.key === "Enter") {
                    if (cmdline_edit.readOnly)
                        return;  // not safe to call main() yet

                    ev.preventDefault();

                    if (status_upd !== null) {
                        clearTimeout(status_upd);
                        status_upd = null;
                    }

                    // handle quoted arguments

                    var args = cmdline_edit.value.match(/'[^']+'|"[^"]+"|[^\s]+/g);

                    if (args === null)
                        args = [];

                    args = args.map((m) => {
                        if ((m.startsWith('\'') && m.endsWith('\''))
                            || (m.startsWith('"') && m.endsWith('"')))
                            return m.substring(1, m.length - 1);
                        else
                            return m;
                    });

                    cmdline_edit.value = '';
                    conout.value = '';  // clear old output
                    status_show.textContent = 'Running';
                    const ex = Module.callMain(args);
                    status_upd = setTimeout(() => {
                        status_show.textContent = `Exited (code ${ex})`;
                    }, 150);
                }
            });
        },
    };
}
