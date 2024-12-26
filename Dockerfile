FROM php:apache

RUN apt-get update -y
RUN apt-get install -y flex build-essential

WORKDIR /var/www/html/
COPY ./src/frontend ./

RUN chmod 644 index.html
RUN chmod 644 searchResults.php

WORKDIR /var/www/html/webpages/
COPY ./src/backend/webpages/ ./
RUN chmod 644 ./*

WORKDIR /backend/
COPY ./src/backend ./

WORKDIR /backend/src/
RUN chmod a+x ./indexCompile.sh
RUN chmod a+x ./queryCompile.sh
RUN ./indexCompile.sh ../webpages ../output
RUN ./queryCompile.sh ../output

WORKDIR /backend/bin/
RUN chmod 755 ./index
RUN chmod 755 ./query

EXPOSE 80

CMD [ "apache2-foreground" ]
