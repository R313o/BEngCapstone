#include "_MULTI_FX.h"



static void fx_null_process(FX_HANDLER_t *fx, pipe *p){

	// empty process

}

static void fx_null_clean(FX_HANDLER_t *fx) {

}



/**
 * @brief empty initialize
 * @param fx Pointer to an FX_HANDLER_t to initialize
 */
void fx_null_init(FX_HANDLER_t *fx) {

	fx->process = fx_null_process;
	fx->clean = fx_null_clean;
}

