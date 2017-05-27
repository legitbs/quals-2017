FROM crystallang/crystal:0.21.1
RUN apt-get update
RUN apt-get install -qqy build-essential ruby
RUN gem install rake --no-rdoc --no-ri

# vito devmode
RUN apt-get install -qqy zsh gdb
RUN gem install pry --no-rdoc --no-ri

ENTRYPOINT zsh
