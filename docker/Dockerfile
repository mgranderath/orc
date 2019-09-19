FROM openwrtorg/sdk:x86-64-18.06.4

RUN ./scripts/feeds update && make defconfig && \
    ./scripts/feeds install libuci && \
    ./scripts/feeds install uhttpd && \
    make package/uci/compile && \
    make package/uhttpd/compile

RUN echo "src-link custom /restconf" > feeds.conf

COPY build.sh build.sh

ENTRYPOINT ["bash", "./build.sh"]
