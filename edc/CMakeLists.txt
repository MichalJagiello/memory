FIND_PROGRAM(EDJE_CC_EXECUTABLE edje_cc DOC "path to edje_cc compiler")
MARK_AS_ADVANCED(EDJE_CC_EXECUTABLE)


IF(EDJE_CC_EXECUTABLE)
	MACRO(EDJ_TARGET Name Edc_Input Edj_Output)
		MESSAGE("-----------EDC COMPILATION---------------")
		MESSAGE("Name: " ${Name})
		MESSAGE("Edc_Input: " ${Edc_Input})
		MESSAGE("Edj_Output: " ${Edj_Output})

		MESSAGE("CMAKE_SOURCE_DIR" ${CMAKE_SOURCE_DIR})
		MESSAGE("CMAKE_BINARY_DIR" ${CMAKE_BINARY_DIR})
		MESSAGE("CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})

		SET(tmp_output ${Edj_Output}/${Name})
		ADD_CUSTOM_COMMAND(OUTPUT ${tmp_output}
					COMMAND ${EDJE_CC_EXECUTABLE}
					ARGS -id ${CMAKE_SOURCE_DIR}/res/images
					${Edc_Input} ${tmp_output}
					COMMENT "[EDJE_CC][${tmp_output}] Compiling EDC file ${Edc_Input}"
					WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

		ADD_CUSTOM_TARGET(${Name} DEPENDS ${tmp_output})
		ADD_DEPENDENCIES(${PROJECT_NAME} ${Name})
		INSTALL(FILES ${tmp_output} DESTINATION res)
		MESSAGE("-----------/EDC COMPILATION---------------")
	ENDMACRO(EDJ_TARGET)

ELSE()
	MESSAGE(FATAL_ERROR "edje_cc tool not found")
ENDIF(EDJE_CC_EXECUTABLE)


EDJ_TARGET(game_grid.edj
	${CMAKE_SOURCE_DIR}/edc/game_grid.edc
	${CMAKE_BINARY_DIR}/res
)

EDJ_TARGET(main_menu.edj
	${CMAKE_SOURCE_DIR}/edc/main_menu.edc
	${CMAKE_BINARY_DIR}/res
)
