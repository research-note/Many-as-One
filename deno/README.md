# Deno based web frontend

Use deno as websocket based frontend webserver interface with users.

```sh
$ deno run --allow-net --allow-read main.ts
```

## Docker build

The CMD line in the Dockerfile determines parameters passed to Deno.

```
CMD ["run", "--allow-net", "main.ts"]
```

Note: That the listen port (8000), must match with the EXPOSED port in the
Dockerfile.

Run using the build and run commands (you may want to use a more descriptive
name than `app`):

```sh
$ docker build -t app . && docker run -it --init -p 8000:8000 app
```

In another terminal or browser you can access:

```sh
$ curl localhost:8000
Hello World
```

## Refernce

- [Deno websocket](https://deno.land/std/examples/chat/server.ts)
- [denoland docker github](https://github.com/denoland/deno_docker)
