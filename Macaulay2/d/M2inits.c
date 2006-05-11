/* this file contains all initializations needed to get going, even if Macaulay2_main() is not called at all */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include <gc.h>
#include "config.h"
#include "M2types.h"
#include "M2inits.h"
#include "M2mem.h"
#include "memdebug.h"
#include "debug.h"
#include "gmp_init.h"
#define TRUE 1
#define FALSE 0

extern void IM2_initialize();

char *progname;
void arginits(int argc, char **argv) { progname = argv[0]; }

static void init_gc(void) {
#if defined(__APPLE__) && defined(__MACH__) && defined(__i386__)
     extern char *get_etext(), *get_end();
     GC_add_roots(get_etext(),get_end());
#endif
     GC_all_interior_pointers = TRUE; /* especially important if MEMDEBUG is on, call first; gc is compiled by default with this on */
#if 0 /* commented out, because we haven't tested this value lately */
     GC_free_space_divisor = 2;
#endif
     GC_INIT();
     if (getenv("GC_free_space_divisor")) {
	  GC_free_space_divisor = atoi(getenv("GC_free_space_divisor"));
	  if (GC_free_space_divisor <= 0) {
	       fprintf(stderr, "%s: non-positive GC_free_space_divisor value, %ld\n", 
		    progname, GC_free_space_divisor);
	       exit (1);
	       }
	  }
     if (getenv("GC_enable_incremental") && atoi(getenv("GC_enable_incremental"))==1) {
	  GC_enable_incremental();
	  fprintf(stderr,"GC_enable_incremental()\n");
	  }
     if (getenv("GC_expand_hp")) {
	  GC_expand_hp(atoi(getenv("GC_expand_hp")));
	  }
#ifdef NDEBUG
     GC_set_warn_proc(dummy_GC_warn_proc);
#endif
     }

#if 0
/* this is the same as getmem(), so use it instead */
void *GC_malloc_function (size_t new) {
     void *p = GC_MALLOC(new);
     if (p == NULL) outofmem();
#    ifdef DEBUG
     trapchk(p);
#    endif
     return p;
     }
#endif

void *GC_realloc_function (void *s, size_t old, size_t new) {
     void *p = GC_REALLOC(s,new);
     if (p == NULL) outofmem();
#    ifdef DEBUG
     trapchk(p);
#    endif
     return p;
     }

void GC_free_function (void *s, size_t old) {
#    ifdef DEBUG
     trapchk(s);
#    endif
     GC_FREE(s);
}

extern int initializeGMP();

static void *(*save_gmp_allocate_func  )(size_t);
static void *(*save_gmp_reallocate_func)(void *, size_t, size_t);
static void  (*save_gmp_free_func      )(void *, size_t);

void enterFactory() {
# if FACTORY
  static int done = 0;
  if (!done) {
    done = 1;
    initializeGMP_Cwrapper();
    save_gmp_allocate_func   = __gmp_allocate_func;
    save_gmp_reallocate_func = __gmp_reallocate_func;
    save_gmp_free_func       = __gmp_free_func;
    }
  else {
    __gmp_allocate_func   = save_gmp_allocate_func;
    __gmp_reallocate_func = save_gmp_reallocate_func;
    __gmp_free_func       = save_gmp_free_func;
  }
# endif
}

void enterM2(void) {
     mp_set_memory_functions( (void *(*) (size_t)) getmem, GC_realloc_function, GC_free_function);
     }

void M2inits(void) {
  static int done = 0;
  if (!done) {
    done = 1;
#   ifdef DEBUG
    trap();			/* we call trap() once so variables (such as trapset) can be set */
#   endif
    init_gc();
    enterM2();
    IM2_initialize();
  }
}

/*
 Local Variables:
 compile-command: "make -C $M2BUILDDIR/Macaulay2/d "
 End:
*/
