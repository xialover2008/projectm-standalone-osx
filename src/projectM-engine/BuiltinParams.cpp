

#include "BuiltinParams.hpp"



BuiltinParams::BuiltinParams() {

    int ret;
    if ((ret = init_builtin_param_db()) != PROJECTM_SUCCESS) {
        throw ret;
    }

}

~BuiltinParams::BuiltinParams() {
   destroy_builtin_param_db();
}

/* Loads a float parameter into the builtin database */
int BuiltinParams::load_builtin_param_float(char * name, void * engine_val, void * matrix, short int flags,
        float init_val, float upper_bound, float lower_bound, char * alt_name) {

    Param * param = NULL;
    CValue iv, ub, lb;

    iv.float_val = init_val;
    ub.float_val = upper_bound;
    lb.float_val = lower_bound;

    /* Create new parameter of type float */
    if (BUILTIN_PARAMS_DEBUG == 2) {
        printf("load_builtin_param_float: (name \"%s\") (alt_name = \"%s\") ", name, alt_name);
        fflush(stdout);
    }

    if ((param = new Param(name, P_TYPE_DOUBLE, flags, engine_val, matrix, iv, ub, lb)) == NULL) {
        return PROJECTM_OUTOFMEM_ERROR;
    }

    if (BUILTIN_PARAMS_DEBUG == 2) {
        printf("created...");
        fflush(stdout);
    }

    /* Insert the paremeter into the database */

    if (insert_builtin_param( param ) < 0) {
        delete param;
        return PROJECTM_ERROR;
    }

    if (BUILTIN_PARAMS_DEBUG == 2) {
        printf("inserted...");
        fflush(stdout);
    }

    /* If this parameter has an alternate name, insert it into the database as link */

    if (alt_name != NULL) {
        insert_param_alt_name(param,alt_name);

        if (BUILTIN_PARAMS_DEBUG == 2) {
            printf("alt_name inserted...");
            fflush(stdout);
        }


    }

    if (BUILTIN_PARAMS_DEBUG == 2) printf("finished\n");

    /* Finished, return success */
    return PROJECTM_SUCCESS;
}



/* Destroy the builtin parameter database.
   Generally, do this on projectm exit */
int BuiltinParams::destroy_builtin_param_db() {

    builtin_param_tree->splay_traverse((void (*)(void*))free_param_helper);
    delete builtin_param_tree;
    builtin_param_tree = NULL;
    return PROJECTM_SUCCESS;
}


/* Insert a parameter into the database with an alternate name */
int BuiltinParams::insert_param_alt_name(BuiltinParams *param, char * alt_name) {

    if (alt_name == NULL)
        return PROJECTM_ERROR;

    builtin_param_tree->splay_insert_link(alt_name, param->name);

    return PROJECTM_SUCCESS;
}

Param * BuiltinParams::find_builtin_param(char * name) {

    /* Null argument checks */
    if (name == NULL)
        return NULL;

    return  (Param*)builtin_param_tree->splay_find(name);

}


/* Loads a integer parameter into the builtin database */
int BuiltinParams::load_builtin_param_int(char * name, void * engine_val, short int flags,
        int init_val, int upper_bound, int lower_bound, char * alt_name) {

    Param * param;
    CValue iv, ub, lb;

    iv.int_val = init_val;
    ub.int_val = upper_bound;
    lb.int_val = lower_bound;

    param = new Param(name, P_TYPE_INT, flags, engine_val, NULL, iv, ub, lb);

    if (param == NULL) {
        return PROJECTM_OUTOFMEM_ERROR;
    }

    if (insert_builtin_param( param ) < 0) {
        delete param;
        return PROJECTM_ERROR;
    }

    if (alt_name != NULL) {
        insert_param_alt_name(param,alt_name);
    }

    return PROJECTM_SUCCESS;

}

/* Loads a boolean parameter */
int BuiltinParams::load_builtin_param_bool(char * name, void * engine_val, short int flags,
        int init_val, char * alt_name) {

    Param * param;
    CValue iv, ub, lb;

    iv.int_val = init_val;
    ub.int_val = TRUE;
    lb.int_val = FALSE;

    param = new Param(name, P_TYPE_BOOL, flags, engine_val, NULL, iv, ub, lb);

    if (param == NULL) {
        return PROJECTM_OUTOFMEM_ERROR;
    }

    if (insert_builtin_param(param) < 0) {
        delete param;
        return PROJECTM_ERROR;
    }

    if (alt_name != NULL) {
        insert_param_alt_name(param,alt_name);
    }

    return PROJECTM_SUCCESS;

}

/* Inserts a parameter into the builtin database */
int BuiltinParams::insert_builtin_param( BuiltinParams *param ) {

    return builtin_param_tree->splay_insert(param, param->name);
}



/* Initialize the builtin parameter database.
   Should only be necessary once */
int BuiltinParams::init_builtin_param_db() {

    /* Create the builtin parameter splay tree (go Sleator...) */
    if ((this->builtin_param_tree = SplayTree::create_splaytree((int (*)(void*,void*))compare_string,(void* (*)(void*)) copy_string, (void (*)(void*))free_string)) == NULL) {
        if (BUILTIN_PARAMS_DEBUG) printf("init_builtin_param_db: failed to initialize database (FATAL)\n");
        return PROJECTM_OUTOFMEM_ERROR;
    }

    if (BUILTIN_PARAMS_DEBUG) {
        printf("init_builtin_param: loading database...");
        fflush(stdout);
    }

    /* Loads all builtin parameters into the database */
    if (BuiltinParams::load_all_builtin_param() < 0) {
        if (BUILTIN_PARAMS_DEBUG) printf("failed loading builtin parameters (FATAL)\n");
        return PROJECTM_ERROR;
    }

    if (BUILTIN_PARAMS_DEBUG) printf("success!\n");

    /* Finished, no errors */
    return PROJECTM_SUCCESS;
}



/* Loads all builtin parameters, limits are also defined here */
int  BuiltinParams::load_all_builtin_param(const PresetInputs & presetInputs, PresetOutputs & presetOutputs) {

  load_builtin_param_float("fRating", (void*)&presetOutputs->fRating, NULL, P_FLAG_NONE, 0.0 , 5.0, 0.0, NULL);
  load_builtin_param_float("fWaveScale", (void*)&presetOutputs->fWaveScale, NULL, P_FLAG_NONE, 0.0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);
  load_builtin_param_float("gamma", (void*)&presetOutputs->fGammaAdj, NULL, P_FLAG_NONE, 0.0, MAX_DOUBLE_SIZE, 0, "fGammaAdj");
  load_builtin_param_float("echo_zoom", (void*)&presetOutputs->fVideoEchoZoom, NULL, P_FLAG_NONE, 0.0, MAX_DOUBLE_SIZE, 0, "fVideoEchoZoom");
  load_builtin_param_float("echo_alpha", (void*)&presetOutputs->fVideoEchoAlpha, NULL, P_FLAG_NONE, 0.0, MAX_DOUBLE_SIZE, 0, "fVideoEchoAlpha");
  load_builtin_param_float("wave_a", (void*)&presetOutputs->fWaveAlpha, NULL, P_FLAG_NONE, 0.0, 1.0, 0, "fWaveAlpha");
  load_builtin_param_float("fWaveSmoothing", (void*)&presetOutputs->fWaveSmoothing, NULL, P_FLAG_NONE, 0.0, 1.0, -1.0, NULL);  
  load_builtin_param_float("fModWaveAlphaStart", (void*)&presetOutputs->fModWaveAlphaStart, NULL, P_FLAG_NONE, 0.0, 1.0, -1.0, NULL);
  load_builtin_param_float("fModWaveAlphaEnd", (void*)&presetOutputs->fModWaveAlphaEnd, NULL, P_FLAG_NONE, 0.0, 1.0, -1.0, NULL);
  load_builtin_param_float("fWarpAnimSpeed",  (void*)&presetOutputs->fWarpAnimSpeed, NULL, P_FLAG_NONE, 0.0, 1.0, -1.0, NULL);
  //  load_builtin_param_float("warp", (void*)&presetOutputs->warp, warp_mesh, P_FLAG_NONE, 0.0, MAX_DOUBLE_SIZE, 0, NULL);
	
  load_builtin_param_float("fShader", (void*)&presetOutputs->fShader, NULL, P_FLAG_NONE, 0.0, 1.0, -1.0, NULL);
  load_builtin_param_float("decay", (void*)&presetOutputs->decay, NULL, P_FLAG_NONE, 0.0, 1.0, 0, "fDecay");

  load_builtin_param_int("echo_orient", (void*)&presetOutputs->nVideoEchoOrientation, P_FLAG_NONE, 0, 3, 0, "nVideoEchoOrientation");
  load_builtin_param_int("wave_mode", (void*)&presetOutputs->nWaveMode, P_FLAG_NONE, 0, 7, 0, "nWaveMode");
  
  load_builtin_param_bool("wave_additive", (void*)&presetOutputs->bAdditiveWaves, P_FLAG_NONE, FALSE, "bAdditiveWaves");
  load_builtin_param_bool("bModWaveAlphaByVolume", (void*)&presetOutputs->bModWaveAlphaByVolume, P_FLAG_NONE, FALSE, NULL);
  load_builtin_param_bool("wave_brighten", (void*)&presetOutputs->bMaximizeWaveColor, P_FLAG_NONE, FALSE, "bMaximizeWaveColor");
  load_builtin_param_bool("wrap", (void*)&presetOutputs->bTexWrap, P_FLAG_NONE, FALSE, "bTexWrap");
  load_builtin_param_bool("darken_center", (void*)&presetOutputs->bDarkenCenter, P_FLAG_NONE, FALSE, "bDarkenCenter");
  load_builtin_param_bool("bRedBlueStereo", (void*)&presetOutputs->bRedBlueStereo, P_FLAG_NONE, FALSE, NULL);
  load_builtin_param_bool("brighten", (void*)&presetOutputs->bBrighten, P_FLAG_NONE, FALSE, "bBrighten");
  load_builtin_param_bool("darken", (void*)&presetOutputs->bDarken, P_FLAG_NONE, FALSE, "bDarken");
  load_builtin_param_bool("solarize", (void*)&presetOutputs->bSolarize, P_FLAG_NONE, FALSE, "bSolarize");
  load_builtin_param_bool("invert", (void*)&presetOutputs->bInvert, P_FLAG_NONE, FALSE, "bInvert");
  load_builtin_param_bool("bMotionVectorsOn", (void*)&presetOutputs->bMotionVectorsOn, P_FLAG_NONE, FALSE, NULL);
  load_builtin_param_bool("wave_dots", (void*)&presetOutputs->bWaveDots, P_FLAG_NONE, FALSE, "bWaveDots");
  load_builtin_param_bool("wave_thick", (void*)&presetOutputs->bWaveThick, P_FLAG_NONE, FALSE, "bWaveThick");

  load_builtin_param_float("zoom", (void*)&presetOutputs->zoom, presetOutputs->zoom_mesh,  P_FLAG_PER_PIXEL |P_FLAG_DONT_FREE_MATRIX, 0.0, MAX_DOUBLE_SIZE, 0, NULL);
  load_builtin_param_float("rot", (void*)&presetOutputs->rot, presetOutputs->rot_mesh,  P_FLAG_PER_PIXEL |P_FLAG_DONT_FREE_MATRIX, 0.0, MAX_DOUBLE_SIZE, MIN_DOUBLE_SIZE, NULL);
  load_builtin_param_float("zoomexp", (void*)&presetOutputs->zoomexp, presetOutputs->zoomexp_mesh,  P_FLAG_PER_PIXEL |P_FLAG_NONE, 0.0, MAX_DOUBLE_SIZE, 0, "fZoomExponent");
 
  load_builtin_param_float("cx", (void*)&presetOutputs->cx, presetOutputs->cx_mesh, P_FLAG_PER_PIXEL | P_FLAG_DONT_FREE_MATRIX, 0.0, 1.0, 0, NULL);
  load_builtin_param_float("cy", (void*)&presetOutputs->cy, presetOutputs->cy_mesh, P_FLAG_PER_PIXEL | P_FLAG_DONT_FREE_MATRIX, 0.0, 1.0, 0, NULL);
  load_builtin_param_float("dx", (void*)&presetOutputs->dx, presetOutputs->dx_mesh,  P_FLAG_PER_PIXEL | P_FLAG_DONT_FREE_MATRIX, 0.0, MAX_DOUBLE_SIZE, MIN_DOUBLE_SIZE, NULL);
  load_builtin_param_float("dy", (void*)&presetOutputs->dy, presetOutputs->dy_mesh,  P_FLAG_PER_PIXEL |P_FLAG_DONT_FREE_MATRIX, 0.0, MAX_DOUBLE_SIZE, MIN_DOUBLE_SIZE, NULL);
  load_builtin_param_float("sx", (void*)&presetOutputs->sx, presetOutputs->sx_mesh,  P_FLAG_PER_PIXEL |P_FLAG_DONT_FREE_MATRIX, 0.0, MAX_DOUBLE_SIZE, 0, NULL);
  load_builtin_param_float("sy", (void*)&presetOutputs->sy, presetOutputs->sy_mesh,  P_FLAG_PER_PIXEL |P_FLAG_DONT_FREE_MATRIX, 0.0, MAX_DOUBLE_SIZE, 0, NULL);

  load_builtin_param_float("wave_r", (void*)&presetOutputs->wave_r, NULL, P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("wave_g", (void*)&presetOutputs->wave_g, NULL, P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("wave_b", (void*)&presetOutputs->wave_b, NULL, P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("wave_x", (void*)&presetOutputs->wave_x, NULL, P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("wave_y", (void*)&presetOutputs->wave_y, NULL, P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("wave_mystery", (void*)&presetOutputs->wave_mystery, NULL, P_FLAG_NONE, 0.0, 1.0, -1.0, "fWaveParam");
  
  load_builtin_param_float("ob_size", (void*)&presetOutputs->ob_size, NULL, P_FLAG_NONE, 0.0, 0.5, 0, NULL);
  load_builtin_param_float("ob_r", (void*)&presetOutputs->ob_r, NULL, P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("ob_g", (void*)&presetOutputs->ob_g, NULL, P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("ob_b", (void*)&presetOutputs->ob_b, NULL, P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("ob_a", (void*)&presetOutputs->ob_a, NULL, P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);

  load_builtin_param_float("ib_size", (void*)&presetOutputs->ib_size,  NULL,P_FLAG_NONE, 0.0, .5, 0.0, NULL);
  load_builtin_param_float("ib_r", (void*)&presetOutputs->ib_r,  NULL,P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("ib_g", (void*)&presetOutputs->ib_g,  NULL,P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("ib_b", (void*)&presetOutputs->ib_b,  NULL,P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("ib_a", (void*)&presetOutputs->ib_a,  NULL,P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);

  load_builtin_param_float("mv_r", (void*)&presetOutputs->mv_r,  NULL,P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("mv_g", (void*)&presetOutputs->mv_g,  NULL,P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("mv_b", (void*)&presetOutputs->mv_b,  NULL,P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);
  load_builtin_param_float("mv_x", (void*)&presetOutputs->mv_x,  NULL,P_FLAG_NONE, 0.0, 64.0, 0.0, "nMotionVectorsX");
  load_builtin_param_float("mv_y", (void*)&presetOutputs->mv_y,  NULL,P_FLAG_NONE, 0.0, 48.0, 0.0, "nMotionVectorsY");
  load_builtin_param_float("mv_l", (void*)&presetOutputs->mv_l,  NULL,P_FLAG_NONE, 0.0, 5.0, 0.0, NULL);
  load_builtin_param_float("mv_dy", (void*)&presetOutputs->mv_dy, NULL, P_FLAG_NONE, 0.0, 1.0, -1.0, NULL);
  load_builtin_param_float("mv_dx", (void*)&presetOutputs->mv_dx,  NULL,P_FLAG_NONE, 0.0, 1.0, -1.0, NULL);
  load_builtin_param_float("mv_a", (void*)&presetOutputs->mv_a,  NULL,P_FLAG_NONE, 0.0, 1.0, 0.0, NULL);

  load_builtin_param_float("time", (void*)&presetInputs->Time,  NULL,P_FLAG_READONLY, 0.0, MAX_DOUBLE_SIZE, 0.0, NULL);
  load_builtin_param_float("bass", (void*)&presetInputs->bass,  NULL,P_FLAG_READONLY, 0.0, MAX_DOUBLE_SIZE, 0.0, NULL);
  load_builtin_param_float("mid", (void*)&bpresetInputs->mid,  NULL,P_FLAG_READONLY, 0.0, MAX_DOUBLE_SIZE, 0, NULL);
  load_builtin_param_float("bass_att", (void*)&presetInputs->bass_att,  NULL,P_FLAG_READONLY, 0.0, MAX_DOUBLE_SIZE, 0, NULL);
  load_builtin_param_float("mid_att", (void*)&presetInputs->mid_att,  NULL, P_FLAG_READONLY, 0.0, MAX_DOUBLE_SIZE, 0, NULL);
  load_builtin_param_float("treb_att", (void*)&presetInputs->treb_att,  NULL, P_FLAG_READONLY, 0.0, MAX_DOUBLE_SIZE, 0, NULL);
  load_builtin_param_int("frame", (void*)&presetInputs->frame, P_FLAG_READONLY, 0, MAX_INT_SIZE, 0, NULL);
  load_builtin_param_float("progress", (void*)&presetInputs->progress,  NULL,P_FLAG_READONLY, 0.0, 1, 0, NULL);
  load_builtin_param_int("fps", (void*)&presetInputs->fps, P_FLAG_READONLY, 15, MAX_INT_SIZE, 0, NULL);

  load_builtin_param_float("x", (void*)&presetInputs>x_per_pixel, presetInputs->x_mesh,  P_FLAG_PER_PIXEL |P_FLAG_ALWAYS_MATRIX | P_FLAG_READONLY | P_FLAG_DONT_FREE_MATRIX, 
			    0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);
  load_builtin_param_float("y", (void*)&presetInputs->y_per_pixel, presetInputs->y_mesh,  P_FLAG_PER_PIXEL |P_FLAG_ALWAYS_MATRIX |P_FLAG_READONLY | P_FLAG_DONT_FREE_MATRIX, 
			    0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);
  load_builtin_param_float("ang", (void*)&presetInputs->ang_per_pixel, presetInputs->theta_mesh,  P_FLAG_PER_PIXEL |P_FLAG_ALWAYS_MATRIX | P_FLAG_READONLY | P_FLAG_DONT_FREE_MATRIX, 
			    0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);
  load_builtin_param_float("rad", (void*)&presetInputs->rad_per_pixel, presetInputs->rad_mesh,  P_FLAG_PER_PIXEL |P_FLAG_ALWAYS_MATRIX | P_FLAG_READONLY | P_FLAG_DONT_FREE_MATRIX,
			    0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);

  load_builtin_param_float("q1", (void*)&presetOutputs->q1,  NULL, P_FLAG_PER_PIXEL |P_FLAG_QVAR, 0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);
  load_builtin_param_float("q2", (void*)&presetOutputs->q2,  NULL, P_FLAG_PER_PIXEL |P_FLAG_QVAR, 0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);
  load_builtin_param_float("q3", (void*)&presetOutputs->q3,  NULL, P_FLAG_PER_PIXEL |P_FLAG_QVAR, 0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);
  load_builtin_param_float("q4", (void*)&presetOutputs->q4,  NULL, P_FLAG_PER_PIXEL |P_FLAG_QVAR, 0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);
  load_builtin_param_float("q5", (void*)&presetOutputs->q5,  NULL, P_FLAG_PER_PIXEL |P_FLAG_QVAR, 0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);
  load_builtin_param_float("q6", (void*)&presetOutputs->q6,  NULL, P_FLAG_PER_PIXEL |P_FLAG_QVAR, 0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);
  load_builtin_param_float("q7", (void*)&presetOutputs->q7,  NULL, P_FLAG_PER_PIXEL |P_FLAG_QVAR, 0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);
  load_builtin_param_float("q8", (void*)&presetOutputs->q8,  NULL, P_FLAG_PER_PIXEL |P_FLAG_QVAR, 0, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, NULL);

  /* variables added in 1.04 */
  load_builtin_param_int("meshx", (void*)&presetInputs->gx, P_FLAG_READONLY, 32, 96, 8, NULL);
  load_builtin_param_int("meshy", (void*)&presetInputs->gy, P_FLAG_READONLY, 24, 72, 6, NULL);

  return PROJECTM_SUCCESS;

}
