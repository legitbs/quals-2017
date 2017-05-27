FROM alpine:3.5
RUN apk add --no-cache python3
RUN mkdir -p /app
ADD magic_server.tar.bz2 /app
ADD runner.py /app/runner.py
WORKDIR /app

CMD /app/runner.py
