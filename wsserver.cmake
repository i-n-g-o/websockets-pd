set(WS_SERVER wsserver)

set(WS_SERVER_SOURCES
  wsserver.h wsserver.cpp
  PdWebsocketServer.h PdWebsocketServer.cpp
)

pd_add_external(${WS_SERVER} "${WS_SERVER_SOURCES}")

scaryws_setup_target(${WS_CLIENT})
target_link_libraries(${WS_SERVER} scaryws)
