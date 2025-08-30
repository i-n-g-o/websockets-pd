set(WS_CLIENT wsclient)

set(WS_CLIENT_SOURCES
  wsclient.h wsclient.cpp
  PdWebsocketClient.h PdWebsocketClient.cpp
)

pd_add_external(${WS_CLIENT} "${WS_CLIENT_SOURCES}")

scaryws_setup_target(${WS_CLIENT})
target_link_libraries(${WS_CLIENT} scaryws)
