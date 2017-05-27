FROM scorpil/rust:nightly
RUN apt-get update
RUN apt-get install -qqy build-essential ruby
RUN gem install rake --no-doc

# vito devmode
RUN apt-get install -qqy zsh gdb
RUN gem install pry --no-doc

ENTRYPOINT zsh
