

add_custom_target(manpage ${DBWTL_MAIN_DOC_DIR}/manpage/genmanpage.sh
${DBWTL_MAIN_DOC_DIR})

add_custom_target(guide ${DBWTL_MAIN_DOC_DIR}/guide/genguide.sh
${DBWTL_MAIN_DOC_DIR})

add_custom_target(rfc ${DBWTL_MAIN_DOC_DIR}/rfc/genrfc.sh
${DBWTL_MAIN_DOC_DIR})

add_custom_target(sdiref ${DBWTL_MAIN_DOC_DIR}/sdiref/gensdiref.sh
${DBWTL_MAIN_DOC_DIR})

