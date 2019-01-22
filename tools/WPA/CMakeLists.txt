#
#if(DEFINED IN_SOURCE_BUILD)
#    set(LLVM_LINK_COMPONENTS BitWriter Core IPO IrReader InstCombine Instrumentation Target Linker Analysis ScalarOpts Support Svf Cudd)
#    add_llvm_tool( wpa wpa.cpp )
#else()
#    llvm_map_components_to_libnames(llvm_libs bitwriter core ipo irreader instcombine instrumentation target linker analysis scalaropts support )
#    add_executable( wpa wpa.cpp )
#
#    target_link_libraries( wpa LLVMSvf LLVMCudd ${llvm_libs} )
#
#    set_target_properties( wpa PROPERTIES
#                           RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
#    install(TARGETS seadsa RUNTIME DESTINATION bin)
#endif()

set(LLVM_LINK_COMPONENTS BitWriter Core IPO IrReader InstCombine Instrumentation
  Target Linker Analysis ScalarOpts Support Svf Cudd)

add_executable( wpa wpa.cpp )
llvm_config (wpa ${LLVM_LINK_COMPONENTS})

install(TARGETS wpa RUNTIME DESTINATION bin)
