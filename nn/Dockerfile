FROM alpine:latest
RUN apk add --no-cache --upgrade \
    libtbb-dev \
    boost-dev

WORKDIR /opt

COPY websocket-server/bazel-bin/main/main websocket-server
COPY simplenet/bazel-bin/main/main simplenet

EXPOSE 8080

# Keep container running for Test.
# CMD exec /bin/sh -c "trap : TERM INT; sleep infinity & wait"
ENTRYPOINT [ "./websocket-server 0.0.0.0 8080 . 5" ]
