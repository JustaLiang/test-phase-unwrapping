#------------------------------------------------------------------------------
#  test_DMWL_unwrapping 
#------------------------------------------------------------------------------
add_executable(	
	test_DMWL_unwrapping 			
	test_DMWL_unwrapping.cpp
)
target_link_libraries(
	test_DMWL_unwrapping 	
	${LIBRARIES}
)

#------------------------------------------------------------------------------
#  test_2FQ_unwrapping 
#------------------------------------------------------------------------------
add_executable(	
	test_2FQ_unwrapping 			
	test_2FQ_unwrapping.cpp
)
target_link_libraries(
	test_2FQ_unwrapping 	
	${LIBRARIES}
)