
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

  # Install pixmaps
  install(FILES filter_small.png
                speaker_small.png
                driver64.png
                opt_enclosure_16.png
                stock_menu_about.png
                driver_small.png
                opt_enclosure_24.png
                stock_plot_crossover_16.png
                filter64.png
                speaker64.png
                stock_plot_crossover_24.png
          DESTINATION share/pixmaps)

endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
