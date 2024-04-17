var Module = {
#if NODERAWFS
    // only for targeting NodeJS

    'preInit': () => {
        // temporary solution for inheriting system environ:
        //   https://github.com/emscripten-core/emscripten/pull/3948#issuecomment-744032264
        Object.assign(Module.ENV, process.env);
    },
#else
    // only for targeting browsers

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

        // filesystem

        const cwd_show = document.getElementById('cwd_show');
        const dirent_tab = document.getElementById('dirent_tab');

        dirent_tab.addEventListener('click', (ev) => {
            if (ev.target.tagName === 'TD') {
                const prev_ent = dirent_tab.querySelector('.mbb_highlight');
                if (prev_ent !== null)
                    prev_ent.classList.remove('mbb_highlight');

                ev.target.classList.add('mbb_highlight');
            }
        });

        function mbb_do_cwd_disp() {
            cwd_show.textContent = Module.FS.cwd();
        }

        function mbb_do_dirent_disp() {
            dirent_tab.innerHTML = '';  // remove old entries
            Module.FS.readdir('.').sort().forEach((ent) => {
                if (ent === '.')
                    return;  // hide current dir

                var ent_name = ent;

                const buf = Module.FS.stat(ent);
                if (Module.FS.isDir(buf.mode)) {
                    ent_name = ent + '/';
                }

                dirent_tab.insertRow().insertCell().textContent = ent_name;
            });
        }

        function mbb_fs_err_ignored(fn) {
            try {
                return fn();
            } catch (exc) {
                if (exc instanceof Module.FS.ErrnoError)
                    return;

                throw exc;
            }
        }

        const mkdir_btn = document.getElementById('mkdir_btn');
        mkdir_btn.addEventListener('click', () => {
            const name = prompt('Name for the new folder:').trim();

            if (name === null || name.length === 0)
                return;

            mbb_fs_err_ignored(() => {
                Module.FS.mkdir(name);
                mbb_do_dirent_disp();
            });
        });

        const imp_btn = document.getElementById('imp_btn');
        imp_btn.addEventListener('click', () => {
            const file_picker = document.createElement('input');
            file_picker.type = 'file';
            file_picker.onchange = (ev) => {
                const f = ev.target.files[0];

                const rder = new FileReader();
                rder.onload = (_) => {
                    const name = prompt('Name for the imported file:', f.name).trim();

                    if (name === null || name.length === 0)
                        return;

                    const data = new Uint8Array(rder.result);

                    mbb_fs_err_ignored(() => {
                        Module.FS.writeFile(name, data);
                        mbb_do_dirent_disp();
                    });
                };
                rder.readAsArrayBuffer(f);
            };
            file_picker.click();
        });

        // start up

        const scr_sel = document.getElementById('scr_sel');  // always show TTY screen on load
        window.location.href = '#scr_tty';

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
            mbb_do_cwd_disp();
            mbb_do_dirent_disp();

            // we can do stuffs now

            scr_sel.disabled = false;

            status_label.textContent = 'Status: ';
            status_show.textContent = 'Idle';

            cmdline_edit.readOnly = false;
        };

        // exec ctrl

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

                var args = cmdline_edit.value.match(/'[^']+'|"[^"]+"|\S+/g);

                if (args === null)
                    args = [];

                args = args.map((m) => {
                    if ((m.startsWith('\'') && m.endsWith('\''))
                        || (m.startsWith('"') && m.endsWith('"')))
                        return m.substring(1, m.length - 1);
                    else
                        return m;
                });

                cmdline_edit.readOnly = true;
                scr_sel.disabled = true;

                cmdline_edit.value = '';
                conout.value = '';  // clear old output
                status_show.textContent = 'Running';

                const ex = Module.callMain(args);

                status_upd = setTimeout(() => {
                    mbb_do_dirent_disp();
                    status_show.textContent = `Exited (code ${ex})`;

                    cmdline_edit.readOnly = false;
                    scr_sel.disabled = false;
                }, 150);
            }
        });

        // env vars

        const env_edit = document.getElementById('env_edit');
        const json = localStorage.getItem('mbb_environ');
        if (json !== null) {
            // restore saved environ

            const environ = JSON.parse(json);
            Object.keys(environ).forEach((k) => {
                const v = environ[k];

                Module.ENV[k] = v;  // pass to emscripten
                env_edit.value += `${k}=${v}\n`;
            });
        }

        const clear_env_btn = document.getElementById('clear_env_btn');
        clear_env_btn.addEventListener('click', () => {
            env_edit.value = '';
        });

        const apply_env_btn = document.getElementById('apply_env_btn');
        apply_env_btn.addEventListener('click', () => {
            const new_environ = {};

            env_edit.value.split('\n').forEach((l) => {
                const l_ = l.trim();

                if (l_.length === 0)
                    return;  // empty line

                const sep = l_.indexOf('=');
                var k = null;
                var v = null;

                if (sep === -1) {
                    // key only, flag vars?
                    k = l_;
                    v = '';
                } else {
                    k = l_.slice(0, sep);
                    v = l_.slice(sep + 1)
                }

                new_environ[k] = v;
            });

            const new_json = JSON.stringify(new_environ);
            localStorage.setItem('mbb_environ', new_json);

            document.body.style.display = 'none';  // hide page when reloading

            // emscripten limitation:
            // setting ENV is only effective before runtime is initialized
            window.location.reload();
        });
    },
#endif
};
