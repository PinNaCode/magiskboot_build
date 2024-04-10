// wildcard matching for file paths in argument could be enabled by
// default if the MinGW runtime is configured with `--enable-wildcard`
// but we explicitly dont want this behavior on Windows
int _dowildcard = 0;
