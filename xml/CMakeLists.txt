if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Install xml files
  install(FILES box1.xml
                crossover1.xml
                crossover2.xml
                default.xml
                vifa.xml
          DESTINATION share/xml)
  # Install frequency response files
  install(FILES morelmdt32s.frf
                peerles115dt-26-72sf.frf
                vifad27tg-05-06.frf
                seasl14rcyp.frf
                vifap21wo-20-08.frf
                seasl21rn4xp.frf
                seast14rcyph.frf
          DESTINATION share/xml)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
