FROM swift:3.0.2

RUN apt-get update

RUN apt-get install -qqy build-essential ruby curl libchicken-dev chicken-bin clang libicu-dev apt-transport-https ca-certificates wget

RUN curl https://dist.crystal-lang.org/apt/setup.sh | bash
RUN apt-get install crystal

RUN gem install rake --no-doc

# vito devmode
RUN apt-get install -qqy zsh gdb
RUN gem install pry --no-doc

ENTRYPOINT zsh
