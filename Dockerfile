FROM resin/armv7hf-debian:jessie
LABEL io.resin.device-type="beaglebone-green-wifi"

RUN apt-get update && apt-get install -y --no-install-recommends \
                less \
                kmod \
                nano \
                net-tools \
                ifupdown \
                iputils-ping \
                i2c-tools \
                usbutils \
                wget \
        && rm -rf /var/lib/apt/lists/*

RUN echo "deb [arch=armhf] http://repos.rcn-ee.net/debian/ jessie main" >> /etc/apt/sources.list
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-key D284E608A4C46402

RUN apt-get update && apt-get install -y build-essential libwebsockets-dev cmake libssl-dev uuid-dev

# Enable init systemd
ENV INITSYSTEM on

COPY . /tracker-demo

WORKDIR /tracker-demo

CMD ["./scripts/containerScript.sh"]
