FROM alpine:3.5
RUN apk add --no-cache build-base ruby python3 chicken
RUN gem install rake --no-doc

# vito devmode
RUN apk add --no-cache zsh gdb
RUN gem install pry --no-doc

ENTRYPOINT zsh
