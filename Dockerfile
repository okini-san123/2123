FROM ubuntu:18.04
RUN apt-get update
#RUN apt-get install sudo

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Moscow
ENV LANG=C.UTF-8

RUN apt-get update && apt-get install -y tzdata locales

RUN ln -fs /usr/share/zoneinfo/$TZ /etc/localtime && dpkg-reconfigure -f noninteractive tzdata
RUN sed -i -e "s/# $LANG UTF-8/$LANG UTF-8/" /etc/locale.gen && locale-gen

#RUN apt-get update && sudo apt-get install gcc -y
#RUN apt-get update && sudo apt-get install sqlite3 -y

#RUN sudo apt-get update && sudo apt-get install -y qtcreator
RUN apt-get install -y build-essential
RUN apt-get install -y qt5-default
RUN apt-get install -y clang
RUN apt-get install -y qt5-qmake
RUN apt-get install -y qtbase5-dev

WORKDIR /server
COPY . /server

#VOLUME /server


#RUN sudo  /server/Server.pro

RUN qmake Server.pro
RUN make
CMD ["./server"]