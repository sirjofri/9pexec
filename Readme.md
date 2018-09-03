9pexec
======

This applications provides a exec shell command which launches the given
application and serves it's default file descriptors via 9p.

It is planned to serve the following filetree:

- `/stdin` - stdin of the application.
- `/stdout` - stdout of the application.
- `/stderr` - stderr of the application.
- `/cmd` - the application with args part.

Usage
-----

`9pexec <port> <application with args>`

It is planned to provide a filename overwrite feature and a default port.
Something like:

```
9pexec -o stdin=input stdout=output stderr=errors <application>
9pexec -o stdin=input stdout=output -p port <application>
9pexec -p port <application>
9pexec <application>
```
