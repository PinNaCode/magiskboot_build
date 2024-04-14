if (typeof window !== 'undefined') {
    // window is present, we are running in a browser

    var Module = {
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
            // bind stdout

            const conout = document.getElementById('conout');
            const _dec = new TextDecoder();

            Module.TTY.stream_ops.write = (_, buff, off, len) => {
                const arr = buff.subarray(off, off + len);
                conout.value += _dec.decode(arr);
                conout.scrollTop = conout.scrollHeight;

                return len;
            }

            // exec ctrl

            const status_label = document.getElementById('status_label').childNodes[0];
            const status_show = document.getElementById('status_show');
            window.onerror = (_) => {
                status_show.textContent = 'Crashed';
            };

            const cmdline_edit = document.getElementById('cmdline_edit');
            const exec_btn = document.getElementById('exec_btn');

            Module.onRuntimeInitialized = () => {
                // set initial cwd to a nice place
                Module.FS.chdir('/home/web_user');

                // we can call main() now
                status_label.textContent = 'Status: ';
                status_show.textContent = 'Ready';
                exec_btn.disabled = false;
            };
            exec_btn.addEventListener('click', () => {
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
                status_show.textContent = `Exited (code ${ex})`;
            });
        },
    };
}
