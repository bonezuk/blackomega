if (${TIGER_SYSTEM_DEPS})

    file(GLOB WWW_CSS_FILES css/*.css)
    install(FILES ${WWW_CSS_FILES} DESTINATION www/css)

    file(GLOB WWW_IMAGES_DIGIT_FILES images/digits/*.png)
    install(FILES ${WWW_IMAGES_DIGIT_FILES} DESTINATION www/images/digits)

    file(GLOB WWW_IMAGES_BASE_FILES images/*.png)
    install(FILES ${WWW_IMAGES_BASE_FILES} DESTINATION www/images)

    file(GLOB WWW_SCRIPT_FILES scripts/*.js)
    install(FILES ${WWW_SCRIPT_FILES} DESTINATION www/scripts)

    file(GLOB WWW_HTML_FILES *.html)
    install(FILES ${WWW_HTML_FILES} DESTINATION www)

endif (${TIGER_SYSTEM_DEPS})
