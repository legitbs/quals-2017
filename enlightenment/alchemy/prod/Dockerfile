FROM crystallang/crystal:0.21.1
RUN apt-get update
RUN apt-get install -qqy python3
RUN mkdir -p /app
ADD alchemy_server.tar.bz2 /app
ADD runner.py /app/runner.py
WORKDIR /app

CMD /app/runner.py
