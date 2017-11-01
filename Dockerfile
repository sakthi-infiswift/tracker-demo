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

RUN apt-get update && apt-get install -y build-essential

# Enable init systemd
ENV INITSYSTEM on

COPY . /infiswift

WORKDIR /infiswift


RUN gcc -std=c11 -DPRINT_LOG_MSG -I./Common -I./hw -I./projects/spi/include -O0 -g3 -Wall -fmessage-length=0 -o projects/spi/arm-build/trackerDemo projects/spi/src/trackerDemo.c Common/utilities/utilities.c hw/BBGW/GPIO/GPIO.c projects/spi/include/*.c Common/lib/jsmn/jsmn.c -lpthread

CMD ["./scripts/enablePins_runTracker.sh"]
