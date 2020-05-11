
/******************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "splatt_cmds.h"
#include "../io.h"
#include "../stats.h"
#include "../sort.h"



/******************************************************************************
 * SPLATT PERMUTE
 *****************************************************************************/
static char perm_args_doc[] = "TENSOR";
static char perm_doc[] =
  "splatt-sort -- Reording the modes of a sparse tensor.\n\n";

static struct argp_option perm_options[] = {
  { "perm", 'p', "PERM", 0, "Desired permutation" },
  { "outfile", 'o', "FILE", 0, "write reordered tensor to file" },
  { 0, 0, 0, 0, "Mode-dependent options:", 1},
  { 0 }
};

typedef struct
{
  char * ifname;
  char * ofname;
  idx_t perm;
  idx_t order;
  idx_t *dim_perm;
  splatt_perm_type type;
  idx_t mode;
} sort_args;

static error_t parse_perm_opt(
  int key,
  char * arg,
  struct argp_state * state)
{
  sort_args *args = state->input;
  switch(key) {
  case 'p':
    args->perm = atoi(arg);
    break;

  case 'd':
    args->order = atoi(arg);
    break;

  case ARGP_KEY_ARG:
    if(args->ifname != NULL) {
      argp_usage(state);
      break;
    }
    args->ifname = arg;
    break;
  case ARGP_KEY_END:
    if(args->ifname == NULL) {
      argp_usage(state);
      break;
    }
  }
  return 0;
}

static struct argp perm_argp =
  {perm_options, parse_perm_opt, perm_args_doc, perm_doc};

int splatt_sort(
    int argc,
    char ** argv)
{
    sort_args args;
    args.ifname = NULL;
    args.ofname = NULL;
    args.perm = 0;
    args.order = 0;
    idx_t *orig3 = (idx_t[3]){0,1,2};
    idx_t perms3[6][3] = {
        {0,1,2},
        {0,2,1},
        {1,0,2},
        {1,2,0},
        {2,0,1},
        {2,1,0},
    };
    char perms_names3[6][4] = {
        "012",
        "021",
        "102",
        "120",
        "201",
        "210",
    };
    idx_t *orig4 = (idx_t[4]){0,1,2,3};
    idx_t perms4[24][4] = {
        {0,1,2,3},
        {0,1,3,2},
        {0,2,1,3},
        {0,2,3,1},
        {0,3,1,2},
        {0,3,2,1},
        {1,0,2,3},
        {1,0,3,2},
        {1,2,0,3},
        {1,2,3,0},
        {1,3,0,2},
        {1,3,2,0},
        {2,0,1,3},
        {2,0,3,1},
        {2,1,0,3},
        {2,1,3,0},
        {2,3,0,1},
        {2,3,1,0},
        {3,0,1,2},
        {3,0,2,1},
        {3,1,0,2},
        {3,1,2,0},
        {3,2,0,1},
        {3,2,1,0},
    };
    char perms_names4[24][5] = {
        "0123",
        "0132",
        "0213",
        "0231",
        "0312",
        "0321",
        "1023",
        "1032",
        "1203",
        "1230",
        "1302",
        "1320",
        "2013",
        "2031",
        "2103",
        "2130",
        "2301",
        "2310",
        "3012",
        "3021",
        "3102",
        "3120",
        "3201",
        "3210",
    };
    idx_t *orig5 = (idx_t[5]){0,1,2,3,4};
    idx_t perms5[120][5] = {
        {0,1,2,3,4},
        {0,1,2,4,3},
        {0,1,3,2,4},
        {0,1,3,4,2},
        {0,1,4,2,3},
        {0,1,4,3,2},
        {0,2,1,3,4},
        {0,2,1,4,3},
        {0,2,3,1,4},
        {0,2,3,4,1},
        {0,2,4,1,3},
        {0,2,4,3,1},
        {0,3,1,2,4},
        {0,3,1,4,2},
        {0,3,2,1,4},
        {0,3,2,4,1},
        {0,3,4,1,2},
        {0,3,4,2,1},
        {0,4,1,2,3},
        {0,4,1,3,2},
        {0,4,2,1,3},
        {0,4,2,3,1},
        {0,4,3,1,2},
        {0,4,3,2,1},
        {1,0,2,3,4},
        {1,0,2,4,3},
        {1,0,3,2,4},
        {1,0,3,4,2},
        {1,0,4,2,3},
        {1,0,4,3,2},
        {1,2,0,3,4},
        {1,2,0,4,3},
        {1,2,3,0,4},
        {1,2,3,4,0},
        {1,2,4,0,3},
        {1,2,4,3,0},
        {1,3,0,2,4},
        {1,3,0,4,2},
        {1,3,2,0,4},
        {1,3,2,4,0},
        {1,3,4,0,2},
        {1,3,4,2,0},
        {1,4,0,2,3},
        {1,4,0,3,2},
        {1,4,2,0,3},
        {1,4,2,3,0},
        {1,4,3,0,2},
        {1,4,3,2,0},
        {2,0,1,3,4},
        {2,0,1,4,3},
        {2,0,3,1,4},
        {2,0,3,4,1},
        {2,0,4,1,3},
        {2,0,4,3,1},
        {2,1,0,3,4},
        {2,1,0,4,3},
        {2,1,3,0,4},
        {2,1,3,4,0},
        {2,1,4,0,3},
        {2,1,4,3,0},
        {2,3,0,1,4},
        {2,3,0,4,1},
        {2,3,1,0,4},
        {2,3,1,4,0},
        {2,3,4,0,1},
        {2,3,4,1,0},
        {2,4,0,1,3},
        {2,4,0,3,1},
        {2,4,1,0,3},
        {2,4,1,3,0},
        {2,4,3,0,1},
        {2,4,3,1,0},
        {3,0,1,2,4},
        {3,0,1,4,2},
        {3,0,2,1,4},
        {3,0,2,4,1},
        {3,0,4,1,2},
        {3,0,4,2,1},
        {3,1,0,2,4},
        {3,1,0,4,2},
        {3,1,2,0,4},
        {3,1,2,4,0},
        {3,1,4,0,2},
        {3,1,4,2,0},
        {3,2,0,1,4},
        {3,2,0,4,1},
        {3,2,1,0,4},
        {3,2,1,4,0},
        {3,2,4,0,1},
        {3,2,4,1,0},
        {3,4,0,1,2},
        {3,4,0,2,1},
        {3,4,1,0,2},
        {3,4,1,2,0},
        {3,4,2,0,1},
        {3,4,2,1,0},
        {4,0,1,2,3},
        {4,0,1,3,2},
        {4,0,2,1,3},
        {4,0,2,3,1},
        {4,0,3,1,2},
        {4,0,3,2,1},
        {4,1,0,2,3},
        {4,1,0,3,2},
        {4,1,2,0,3},
        {4,1,2,3,0},
        {4,1,3,0,2},
        {4,1,3,2,0},
        {4,2,0,1,3},
        {4,2,0,3,1},
        {4,2,1,0,3},
        {4,2,1,3,0},
        {4,2,3,0,1},
        {4,2,3,1,0},
        {4,3,0,1,2},
        {4,3,0,2,1},
        {4,3,1,0,2},
        {4,3,1,2,0},
        {4,3,2,0,1},
        {4,3,2,1,0},
    };
    char perms_names5[120][6] = {
        "01234",
        "01243",
        "01324",
        "01342",
        "01423",
        "01432",
        "02134",
        "02143",
        "02314",
        "02341",
        "02413",
        "02431",
        "03124",
        "03142",
        "03214",
        "03241",
        "03412",
        "03421",
        "04123",
        "04132",
        "04213",
        "04231",
        "04312",
        "04321",
        "10234",
        "10243",
        "10324",
        "10342",
        "10423",
        "10432",
        "12034",
        "12043",
        "12304",
        "12340",
        "12403",
        "12430",
        "13024",
        "13042",
        "13204",
        "13240",
        "13402",
        "13420",
        "14023",
        "14032",
        "14203",
        "14230",
        "14302",
        "14320",
        "20134",
        "20143",
        "20314",
        "20341",
        "20413",
        "20431",
        "21034",
        "21043",
        "21304",
        "21340",
        "21403",
        "21430",
        "23014",
        "23041",
        "23104",
        "23140",
        "23401",
        "23410",
        "24013",
        "24031",
        "24103",
        "24130",
        "24301",
        "24310",
        "30124",
        "30142",
        "30214",
        "30241",
        "30412",
        "30421",
        "31024",
        "31042",
        "31204",
        "31240",
        "31402",
        "31420",
        "32014",
        "32041",
        "32104",
        "32140",
        "32401",
        "32410",
        "34012",
        "34021",
        "34102",
        "34120",
        "34201",
        "34210",
        "40123",
        "40132",
        "40213",
        "40231",
        "40312",
        "40321",
        "41023",
        "41032",
        "41203",
        "41230",
        "41302",
        "41320",
        "42013",
        "42031",
        "42103",
        "42130",
        "42301",
        "42310",
        "43012",
        "43021",
        "43102",
        "43120",
        "43201",
        "43210",
    };

    argp_parse(&perm_argp, argc, argv, ARGP_IN_ORDER, 0, &args);
    sptensor_t * tt = tt_read(args.ifname);
    if(tt == NULL) {
        return SPLATT_ERROR_BADINPUT;
    }

  int order = tt->nmodes;
  if(order == 3 && args.perm < 6){	
    // Repeat 100 times.
	for(int i = 0; i < 100; i ++){
	    tt_sort(tt, 0, perms3[0]);

    	/* perform permutation */
    	init_timers();
    	idx_t m = perms3[args.perm];
	
    	tt_sort_ksadilla(tt, m, perms3[args.perm], KSadilla, 3);
    	double t = timers[TIMER_SORT].seconds;
        printf("%s | %s | splatt | splatt | splatt", args.ifname, perms_names3[args.perm]);
    	printf(" | %.17g ", t*1000); 
    	printf("\n");
    }
  }

  if(order == 4 && args.perm < 24){
	for(int i = 0; i < 100; i ++){
   	    tt_sort(tt, 0, &perms4[0]);
    	/* perform permutation */
    	init_timers();
    	idx_t m = perms4[args.perm];
    	tt_sort_ksadilla(tt, m, perms4[args.perm], KSadilla, 4);
    	double t = timers[TIMER_SORT].seconds;
        printf("%s | %s | splatt | splatt | splatt", args.ifname, perms_names4[args.perm]);
    	printf(" | %0.17g ", t*1000); 
    	printf("\n");
	}
  }
  
  if(order == 5 && args.perm < 120){
  	for(int i = 0; i < 100; i ++){
	    tt_sort(tt, 0, perms5[0]);
    	/* perform permutation */
    	init_timers();
    	idx_t m = perms5[args.perm];
    	tt_sort_ksadilla(tt, m, perms5[args.perm], Radix, -1);
    	double t = timers[TIMER_SORT].seconds;
        printf("%s | %s | splatt | splatt | splatt", args.ifname, perms_names5[args.perm]);
    	printf(" | %0.17g ", t*1000); 
    	printf("\n");
	}
  }

// // For Debugging: print out tensor
//   for(int i = 0; i < tt->nnz; i ++){
//       for(int j = 0; j < order; j ++ ){
//           printf("%d ", tt->ind[j][i]);
//       }
//       printf("= %g\n", tt->vals[i]);
//   }

  tt_free(tt);

  return EXIT_SUCCESS;
}



