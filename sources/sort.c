/*
  	#[ Includes : sort.c

	Sort routines according to new conventions (25-jun-1997).
	This is more object oriented.
	The active sort is indicated by AR.SS which should agree with
	FunSorts[AR.sLevel];
*/
#define NEWSPLITMERGE

#include "form3.h"
#ifdef PARALLEL /* [20oct1997 ar] */
#include "parallel.h"   
#endif /* PARALLEL [20oct1997 ar] */
/*
  	#] Includes :
  	#[ SortVariables :
*/

#ifdef ANSI
static LONG ComPress(WORD **,LONG *);
static VOID StageSort(FILEHANDLE *);
#else
static LONG ComPress();
static VOID StageSort();
#endif
 
static WORD **SplitScratch = 0;
static LONG InScratch = 0, SplitScratchSize = 0;
 
static SORTING **FunSorts = 0;
static int NumFunSorts = 0;
static int MaxFunSorts = 0;
 
static UWORD *SoScratC = 0;

/*
  	#] SortVariables :
	#[ SortUtilities :
 		#[ WriteStats :				VOID WriteStats(lspace,par)

		par = 0 after a splitmerge.
		par = 1 after merge to sortfile.
		par = 2 after the sort

		current expression is to be found in AS.CurExpr.
		terms are in S->TermsLeft.
		S->GenTerms.

*/

extern LONG ninterms;
char *toterms[] = { "   ", " >>", "-->" };

VOID
WriteStats ARG2(POSITION *,plspace,WORD,par)
{
	LONG millitime, y = MAXLONG >> 1;
	WORD timepart;
	SORTING *S;
	POSITION pp;
	if ( AR.SS == AM.S0 && AC.StatsFlag ) {
		if ( Expressions == 0 ) return;
		S = AR.SS;
		if ( AC.ShortStats ) {}
		else { MesPrint(""); }
		millitime = TimeCPU(1);
		timepart = (WORD)(millitime%1000);
		millitime /= 1000;
		timepart /= 10;
		if ( AC.ShortStats ) {
			if ( par == 0 || par == 2 ) {
				SETBASEPOSITION(pp,y);
				if ( ISLESSPOS(*plspace,pp) ) {
					MesPrint("%7l.%2is %8l>%10l%3s%10l:%10p %s %s",
					millitime,timepart,ninterms,S->GenTerms,toterms[par],
					S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
/*
					MesPrint("%14s %17s %7l.%2is %8l>%10l%3s%10l:%10p",
					EXPRNAME(AS.CurExpr),AC.Commercial,millitime,timepart,
					ninterms,S->GenTerms,toterms[par],S->TermsLeft,plspace);
*/
				}
				else {
					LONG y = 1000000000L;
					SETBASEPOSITION(pp,y);
					MULPOS(pp,100);
					if ( ISLESSPOS(*plspace,pp) ) {
						MesPrint("%7l.%2is %8l>%10l%3s%10l:%11p %s %s",
						millitime,timepart,ninterms,S->GenTerms,toterms[par],
						S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
					}
					else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %8l>%10l%3s%10l:%12p %s %s",
							millitime,timepart,ninterms,S->GenTerms,toterms[par],
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %8l>%10l%3s%10l:%13p %s %s",
							millitime,timepart,ninterms,S->GenTerms,toterms[par],
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %8l>%10l%3s%10l:%14p %s %s",
							millitime,timepart,ninterms,S->GenTerms,toterms[par],
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %8l>%10l%3s%10l:%15p %s %s",
							millitime,timepart,ninterms,S->GenTerms,toterms[par],
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %8l>%10l%3s%10l:%16p %s %s",
							millitime,timepart,ninterms,S->GenTerms,toterms[par],
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %8l>%10l%3s%10l:%17p %s %s",
							millitime,timepart,ninterms,S->GenTerms,toterms[par],
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						} } } } }
					}
				}
			}
			else if ( par == 1 ) {
				SETBASEPOSITION(pp,y);
				if ( ISLESSPOS(*plspace,pp) ) {
					MesPrint("%7l.%2is %10l:%10p",millitime,timepart,
					S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
/*
					MesPrint("%14s %17s %7l.%2is %10l:%10p",
					EXPRNAME(AS.CurExpr),AC.Commercial,millitime,timepart,
					S->TermsLeft,plspace);
*/
				}
				else {
					LONG y = 1000000000L;
					SETBASEPOSITION(pp,y);
					MULPOS(pp,100);
					if ( ISLESSPOS(*plspace,pp) ) {
						MesPrint("%7l.%2is %10l:%11p",millitime,timepart,
						S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
					}
					else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %10l:%12p",millitime,timepart,
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %10l:%13p",millitime,timepart,
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %10l:%14p",millitime,timepart,
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %10l:%15p",millitime,timepart,
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %10l:%16p",millitime,timepart,
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						else {
						MULPOS(pp,10);
						if ( ISLESSPOS(*plspace,pp) ) {
							MesPrint("%7l.%2is %10l:%17p",millitime,timepart,
							S->TermsLeft,plspace,EXPRNAME(AS.CurExpr),AC.Commercial);
						}
						} } } } }
					}
				}
			}
		}
		else {
		if ( par == 1 ) {
			MesPrint("Time = %7l.%2i sec",millitime,timepart);
		}
		else {
			MesPrint("Time = %7l.%2i sec    Generated terms = %10l",
				millitime,timepart,S->GenTerms);
		}
		if ( par == 0 )
			MesPrint(" %16s%7l Terms %s = %10l",EXPRNAME(AS.CurExpr),
			ninterms,FG.swmes[par],S->TermsLeft);
		else
			MesPrint(" %16s        Terms %s = %10l",
			EXPRNAME(AS.CurExpr),FG.swmes[par],S->TermsLeft);
		SETBASEPOSITION(pp,y);
		if ( ISLESSPOS(*plspace,pp) ) {
			MesPrint("%24s Bytes used      = %10p",AC.Commercial,plspace);
		}
		else {
			LONG y = 1000000000L;
			SETBASEPOSITION(pp,y);
			MULPOS(pp,100);
			if ( ISLESSPOS(*plspace,pp) ) {
				MesPrint("%24s Bytes used      =%11p",AC.Commercial,plspace);
			}
			else {
				MULPOS(pp,10);
				if ( ISLESSPOS(*plspace,pp) ) {
				MesPrint("%24s Bytes used     =%12p",AC.Commercial,plspace);
				}
				else {
				MULPOS(pp,10);
				if ( ISLESSPOS(*plspace,pp) ) {
				MesPrint("%24s Bytes used    =%13p",AC.Commercial,plspace);
				}
				else {
				MULPOS(pp,10);
				if ( ISLESSPOS(*plspace,pp) ) {
				MesPrint("%24s Bytes used   =%14p",AC.Commercial,plspace);
				}
				else {
				MULPOS(pp,10);
				if ( ISLESSPOS(*plspace,pp) ) {
				MesPrint("%24s Bytes used  =%15p",AC.Commercial,plspace);
				}
				else {
				MULPOS(pp,10);
				if ( ISLESSPOS(*plspace,pp) ) {
				MesPrint("%24s Bytes used =%16p",AC.Commercial,plspace);
				}
				else {
				MULPOS(pp,10);
				if ( ISLESSPOS(*plspace,pp) ) {
				MesPrint("%24s Bytes used=%17p",AC.Commercial,plspace);
				}
				} } } } }
			}
		} }
	}
}

/*
 		#] WriteStats :
 		#[ NewSort :				WORD NewSort()

		Starts a new sort.
		At the lowest level this is a 'main sort' with the struct according
		to the parameters in S0. At higher levels this is a sort for
		functions, subroutines or dollars.
		We prepare the arrays and structs.
*/

WORD
NewSort()
{
	SORTING *S, **newFS;
	int i, newsize;
	if ( SoScratC == 0 )
		SoScratC = (UWORD *)Malloc1(2*(AM.MaxTal+2)*sizeof(UWORD),"NewSort");
	AR.sLevel++;
	if ( AR.sLevel >= NumFunSorts ) {
		if ( NumFunSorts == 0 ) newsize = 100;
		else newsize = 2*NumFunSorts;
		newFS = (SORTING **)Malloc1((newsize+1)*sizeof(SORTING *),"FunSort pointers");
		for ( i = 0; i < NumFunSorts; i++ ) newFS[i] = FunSorts[i];
		for ( ; i <= newsize; i++ ) newFS[i] = 0;
		if ( FunSorts ) M_free(FunSorts,"FunSort pointers");
		FunSorts = newFS; NumFunSorts = newsize;
	}
	if ( AR.sLevel == 0 ) { FunSorts[0] = AM.S0; }
	else {
		if ( FunSorts[AR.sLevel] == 0 ) {
			FunSorts[AR.sLevel] = AllocSort(
				AM.SLargeSize,AM.SSmallSize,AM.SSmallEsize,AM.STermsInSmall
					,AM.SMaxPatches,AM.SMaxFpatches,AM.SIOsize);
		}
	}
	AR.SS = S = FunSorts[AR.sLevel];
	S->sFill = S->sBuffer;
	S->lFill = S->lBuffer;
	S->lPatch = 0;
	S->fPatchN = 0;
	S->GenTerms = S->TermsLeft = S->GenSpace = S->SpaceLeft = 0;
	S->PoinFill = S->sPointer;
	*S->PoinFill = S->sFill;
	PUTZERO(S->SizeInFile[0]); PUTZERO(S->SizeInFile[1]); PUTZERO(S->SizeInFile[2]);
	S->sTerms = 0;
	PUTZERO(S->file.POposition);
	S->stage4 = 0;
	if ( AR.sLevel > MaxFunSorts ) MaxFunSorts = AR.sLevel;
/*
	The next variable is for the staged sort only.
	It should be treated differently

	AR.OldPosOut = 0;
*/
	return(0);
}

/*
 		#] NewSort :
 		#[ EndSort :				WORD EndSort(buffer,par)

		Finishes a sort.
		At AR.sLevel == 0 the output is to the regular output stream.
		When AR.sLevel > 0, the parameter par determines the actual output.
		The AR.sLevel will be popped.
		All ongoing stages are finished and if the sortfile is open
		it is closed.
		The statistics are printed when AR.sLevel == 0.
		par == 0  Output to the buffer.
		par == 1  Output can be obtained term by term from another routine.
		          The filehandle to that routine will be obtained from
		          buffer which will be treated as FILEHANDLE**.
		par == 2  Sort for $-variable. We return the address of the buffer
		          that contains the output in buffer (treated like WORD **).
		          We first catch the output in a file (unless we can
		          intercept things after the small buffer has been sorted)
		          Then we read from the file into a buffer.
*/

LONG
EndSort ARG2(WORD *,buffer,int,par)
{
  SORTING *S = AR.SS;
  WORD j, **ss, *to, *t;
  LONG sSpace, over, tover, spare, retval = 0;
  POSITION position, pp;
  off_t lSpace;
  FILEHANDLE *fout, *oldoutfile, *newout = 0;

  if ( AR.exitflag && AR.sLevel == 0 ) return(0);
#ifdef PARALLEL 
  if( (retval = PF_EndSort()) > 0){
	oldoutfile = AR.outfile;
	retval = 0;
	goto RetRetval;
  } 
  else if(retval < 0){
	retval = -1; 
	goto RetRetval; 
  }
  else{ /* retval = 0: for S != AM.S0 and slaves still do the regular sort */
#endif /* PARALLEL */
	oldoutfile = AR.outfile;
	if ( par == 1 ) { AR.outfile = newout = AllocFileHandle(); }
	if ( S == AM.S0 ) S->PolyFlag = ( AC.PolyFun != 0 ) ? 1: 0;
	else { S->PolyFlag = S->PolyWise = 0; }
	*(S->PoinFill) = 0;

	SplitMerge(S->sPointer,S->sTerms);

	sSpace = 0;
	tover = over = S->sTerms;
	ss = S->sPointer;
	if ( over >= 0 ) {
		if ( S->lPatch > 0 || S->file.handle >= 0 ) {
			ss[over] = 0;
			sSpace = ComPress(ss,&spare);
			S->TermsLeft -= over - spare;
		}
		else if ( S != AM.S0 && ( par == 0 || par == 2 ) ) {
			ss[over] = 0;
			if ( par == 2 ) {
				sSpace = 1;
				while ( ( t = *ss++ ) != 0 ) { sSpace += *t; }
				to = (WORD *)Malloc1(sSpace*sizeof(WORD),"$-sort space");
				*((WORD **)buffer) = to;
				ss = S->sPointer;
				while ( ( t = *ss++ ) != 0 ) {
					j = *t; while ( --j >= 0 ) *to++ = *t++;
				}
				*to = 0;
				retval = sSpace + 1;
			}
			else {
				to = buffer;
				sSpace = 0;
				while ( ( t = *ss++ ) != 0 ) {
					j = *t;
					if ( ( sSpace += j ) > AM.MaxTer ) {
						MesPrint("Sorted function argument too long.");
						retval = -1; goto RetRetval;
					}
					while ( --j >= 0 ) *to++ = *t++;
				}
				*to = 0;
			}
			goto RetRetval;
		}
		else {
			POSITION oldpos;
			if ( S == AM.S0 ) {
				fout = AR.outfile;
				*AR.CompressPointer = 0;
				SeekScratch(AR.outfile,&position);
			}
			else {
				fout = &(S->file);
				PUTZERO(position);
			}
			oldpos = position;
			S->TermsLeft = 0;
			if ( tover > 0 ) {
				ss = S->sPointer;
				while ( ( t = *ss++ ) != 0 ) {
					if ( *t ) S->TermsLeft++;
					if ( PutOut(t,&position,fout,1) < 0 ) {
						retval = -1; goto RetRetval;
					}
				}
			}
			if ( FlushOut(&position,fout) ) {
				retval = -1; goto RetRetval;
			}
			DIFPOS(oldpos,position,oldpos);
			S->SpaceLeft = BASEPOSITION(oldpos);
			WriteStats(&oldpos,(WORD)2);
			goto RetRetval;
		}
	}
	sSpace++;
	lSpace = sSpace + (S->lFill - S->lBuffer) - (LONG)S->lPatch*AM.MaxTer;
	SETBASEPOSITION(pp,lSpace);
	MULPOS(pp,sizeof(WORD));
	if ( S->file.handle >= 0 ) {
		ADD2POS(pp,S->fPatches[S->fPatchN]);
	}
	if ( S == AM.S0 ) {
		WORD oldLogHandle = AC.LogHandle;
		if ( AC.LogHandle >= 0 && AM.LogType && ( ( S->lPatch > 0 )
			|| S->file.handle >= 0 ) ) AC.LogHandle = -1;
		if ( S->lPatch > 0 || S->file.handle >= 0 ) { WriteStats(&pp,0); }
		AC.LogHandle = oldLogHandle;
	}
	if ( par == 2 ) { AR.outfile = newout = AllocFileHandle(); }
	if ( S->lPatch > 0 ) {
		if ( ( S->lPatch >= S->MaxPatches ) ||
			( ( S->lFill + sSpace + 2*AM.MaxTer ) >= S->lTop ) ) {
/*
			The large buffer is too full. Merge and write it
*/
			if ( MergePatches(1) ) {
				MesCall("EndSort"); retval = -1; goto RetRetval;
			}
			S->lPatch = 0;
			pp = S->SizeInFile[1];
			ADDPOS(pp,sSpace);
			MULPOS(pp,sizeof(WORD));
			if ( S == AM.S0 ) {
				WORD oldLogHandle = AC.LogHandle;
				if ( AC.LogHandle >= 0 && AM.LogType ) AC.LogHandle = -1;
				WriteStats(&pp,(WORD)1);
				AC.LogHandle = oldLogHandle;
			}
		}
		else {
			S->Patches[S->lPatch++] = S->lFill;
			to = S->lFill + AM.MaxTer;
			if ( tover > 0 ) {
				ss = S->sPointer;
				while ( ( t = *ss++ ) != 0 ) {
					j = *t;
					if ( j < 0 ) j = t[1] + 2;
					while ( --j >= 0 ) *to++ = *t++;
				}
			}
			*to++ = 0;
			S->lFill = to;
			if ( S->file.handle < 0 ) {
				if ( MergePatches(2) ) {
					MesCall("EndSort"); retval = -1; goto RetRetval;
				}
				pp = S->SizeInFile[2];
				MULPOS(pp,sizeof(WORD));
				WriteStats(&pp,2);
				goto RetRetval;
			}
			if ( MergePatches(1) ) {
				MesCall("EndSort"); retval = -1; goto RetRetval;
			}
			pp = S->SizeInFile[1];
			ADDPOS(pp,sSpace);
			MULPOS(pp,sizeof(WORD));
			if ( S == AM.S0 ) {
				WORD oldLogHandle = AC.LogHandle;
				if ( AC.LogHandle >= 0 && AM.LogType ) AC.LogHandle = -1;
				WriteStats(&pp,(WORD)1);
				AC.LogHandle = oldLogHandle;
			}
		}
	}
	if ( S->file.handle >= 0 ) {
		if ( S->lPatch <= 0 ) {
			StageSort(&(S->file));
			position = S->fPatches[S->fPatchN];
			ss = S->sPointer;
			while ( ( t = *ss++ ) != 0 ) {
				if ( PutOut(t,&position,&(S->file),0) < 0 ) {
					retval = -1; goto RetRetval;
				}
			}
			if ( FlushOut(&position,&(S->file)) ) {
				retval = -1; goto RetRetval;
			}
			++(S->fPatchN); /* ?????????????????????*/
			S->fPatches[S->fPatchN] = position;
		}
		AR.Stage4Name = 0;
		if ( MergePatches(0) ) {
			MesCall("EndSort"); retval = -1; goto RetRetval;
		}
		S->stage4 = 0;
		pp = S->SizeInFile[0];
		MULPOS(pp,sizeof(WORD));
		WriteStats(&pp,2);
	}
RetRetval:
/*[25nov2003 mt]:*/
/*For parallel sorting S->TermsLeft is 0, so we have to use PF.goutterms instead:*/

#ifdef REMOVEDBY_MT
#ifdef PARALLEL /* [11mar1998 ar] */
	if ( AR.sLevel == 0 && PF.me == MASTER) {
#else
	if ( AR.sLevel == 0 ) {
#endif /* PARALLEL [11mar1998 ar] */
		Expressions[AS.CurExpr].counter = S->TermsLeft;
	}
#endif

#ifdef PARALLEL
	if ( AR.sLevel == 0 && PF.me == MASTER) {
		if(AC.mparallelflag == PARALLELFLAG)
			Expressions[AS.CurExpr].counter = PF.goutterms;
		else
			Expressions[AS.CurExpr].counter = S->TermsLeft;
	}/*if ( AR.sLevel == 0 && PF.me == MASTER)*/
#else
	if ( AR.sLevel == 0 ) {
		Expressions[AS.CurExpr].counter = S->TermsLeft;
	}/*if ( AR.sLevel == 0 )*/
#endif
/*:[25nov2003 mt]*/
	if ( S->file.handle >= 0 && ( par != 1 ) && ( par != 2 ) ) {
				/* sortfile is still open */
		CloseFile(S->file.handle);
		S->file.handle = -1;
		remove(S->file.name);
	}
	AR.outfile = oldoutfile;
	AR.sLevel--;
	if ( AR.sLevel >= 0 ) AR.SS = FunSorts[AR.sLevel];
	if ( par == 1 ) {
		if ( retval < 0 ) { DeAllocFileHandle(newout);
/*			if ( AR.outfile->handle >= 0 ) {
				CloseFile(newout->handle);
				remove(newout->name);
			}
			M_free(newout,"FileHandle"); */
		}
		else *((FILEHANDLE **)buffer) = newout;
	}
	else if ( par == 2 ) {
		if ( newout ) {
			if ( retval == 0 ) {
			  if ( newout->handle >= 0 ) {
/*
				output resides at the moment in a file
				Find the size, make a buffer, copy into the buffer and clean up.
*/
				POSITION zeropos;
				PUTZERO(position);
				SeekFile(newout->handle,&position,SEEK_END);
				PUTZERO(zeropos);
				SeekFile(newout->handle,&zeropos,SEEK_SET);
				to = (WORD *)Malloc1(BASEPOSITION(position)+sizeof(WORD)
						,"$-buffer reading");
/* --COMPRESS--? */
				if ( ( retval = ReadFile(newout->handle,(UBYTE *)to,BASEPOSITION(position)) ) !=
				BASEPOSITION(position) ) {
					MesPrint("Error reading information for $ variable");
					M_free(to,"$-buffer reading");
					retval = -1;
				}
				else {
					*((WORD **)buffer) = to;
					retval /= sizeof(WORD);
				}
			  }
			  else {
/*
				output resides in the cache buffer and the file was never opened
*/
				LONG wsiz = newout->POfill - newout->PObuffer;
				to = (WORD *)Malloc1(wsiz*sizeof(WORD),"$-buffer reading");
				*((WORD **)buffer) = to; t = newout->PObuffer;
				retval = wsiz;
				NCOPY(to,t,wsiz);
			  }
			}
			DeAllocFileHandle(newout);
/*
			if ( newout->handle >= 0 ) {
				CloseFile(newout->handle);
				newout->handle = -1;
				remove(newout->name);
			}
			M_free(newout,"Temporary FileHandle");
*/
		}
	}
	return(retval);
#ifdef PARALLEL /* [27aug1997 ar] */
  }
#endif /* PARALLEL [27aug1997 ar] */
}

/*
 		#] EndSort :
 		#[ PutIn :					LONG PutIn(handle,position,buffer,take)

	Reads a new patch from position in file handle.
	It is put at buffer, anything after take is moved forward.

*/

LONG
PutIn ARG4(FILEHANDLE *,file,POSITION *,position,WORD *,buffer,WORD **,take)
{
	LONG i, RetCode;
	WORD *from, *to;
	SeekFile(file->handle,position,SEEK_SET);
	from = buffer + ( file->POsize * sizeof(UBYTE) )/sizeof(WORD);
	i = from - *take;
	if ( i > AM.MaxTer ) {
		MesPrint("Problems in PutIn");
		Terminate(-1);
	}
	to = buffer;
	while ( --i >= 0 ) *--to = *--from;
	*take = to;
/* --COMPRESS-- */
	if ( ( RetCode = ReadFile(file->handle,(UBYTE *)buffer,file->POsize) ) < 0 ) {
		MesPrint("PutIn: We have RetCode = %x while reading %x bytes",
			RetCode,file->POsize);
		Terminate(-1);
	}
	return(RetCode);
}

/*
 		#] PutIn :
 		#[ Sflush :					WORD Sflush(file)

	Puts the contents of a buffer to output

*/

WORD
Sflush ARG1(FILEHANDLE *,fi)
{
	LONG size, RetCode;
	if ( fi->handle < 0 ) {
		if ( ( RetCode = CreateFile(fi->name) ) >= 0 ) {
			fi->handle = (WORD)RetCode;
			PUTZERO(fi->filesize);
			PUTZERO(fi->POposition);
		}
		else return(MesPrint("Cannot create scratch file %s",fi->name));
	}
	else { SeekFile(fi->handle,&(fi->POposition),SEEK_SET); }
	size = (fi->POfill-fi->PObuffer)*sizeof(WORD);
/* --COMPRESS-- */
	if ( WriteFile(fi->handle,(UBYTE *)(fi->PObuffer),size) != size ) {
		return(MesPrint("Write error while finishing sort. Disk full?"));
	}
	ADDPOS(fi->filesize,size);
	ADDPOS(fi->POposition,size);
	fi->POfill = fi->PObuffer;
	return(0);
}

/*
 		#] Sflush :
 		#[ PutOut :					WORD PutOut(term,position,file)

	Routine writes one term to file handle at position. It returns
	the new value of the position.

	NOTE:
		For 'final output' we may have to index the brackets.
		See the struct BRACKETINDEX.
		We should maintain:
		1: a list with brackets
			array with the brackets
		2: a list of objects of type BRACKETINDEX. It contains
			array with either pointers or offsets to the list of brackets.
			starting positions in the file.
		The index may be tied to a maximum size. In that case we may have to
		prune the list occasionally.
*/
WORD
PutOut ARG4(WORD *,term,POSITION *,position,FILEHANDLE *,fi,WORD,ncomp)
{
	WORD i, *p, ret, *r, *rr, j, k, first;
	LONG RetCode;

	if ( AR.SS != AM.S0 ) {
		if ( ( i = *term ) <= 0 ) return(0);
		ret = i;
		ADDPOS(*position,i*sizeof(WORD));
		p = fi->POfill;
		do {
			if ( p >= fi->POstop ) {
				if ( fi->handle < 0 ) {
					if ( ( RetCode = CreateFile(fi->name) ) >= 0 ) {
						fi->handle = (WORD)RetCode;
						PUTZERO(fi->filesize);
					}
					else {
						MesPrint("Cannot create scratch file %s",fi->name);
						return(-1);
					}
				}
				SeekFile(fi->handle,&(fi->POposition),SEEK_SET);
#ifdef WITHGZIP
/*
				Here we  --COMPRESS--  the fi->POsize bytes in fi->PObuffer
				Then we write them. We have to record that block i went
				to location fi->filesize and that the block has the compressed
				length. We update fi->filesize and fi->POposition.
				Note that the block sizes should be part of the fi struct.
*/
#else
				if ( WriteFile(fi->handle,(UBYTE *)(fi->PObuffer),fi->POsize) != fi->POsize ) {
					return(MesPrint("Write error during sort. Disk full?"));
				}
				ADDPOS(fi->filesize,fi->POsize);
				p = fi->PObuffer;
				ADDPOS(fi->POposition,fi->POsize);
#endif
			} 
			*p++ = *term++;
		} while ( --i > 0 );
		fi->POfill = p;
		return(ret);
	}
	if ( ( AC.PreDebug & DUMPOUTTERMS ) == DUMPOUTTERMS ) {
			PrintTerm(term,"PutOut");
			MesPrint("ncomp = %d, AR.NoCompress = %d, AR.sLevel = %d",ncomp,AR.NoCompress,AR.sLevel);
	}
	r = rr = AR.CompressPointer;
	first = j = k = ret = 0;
	if ( ( i = *term ) != 0 ) {
		if ( i < 0 ) {			/* Compressed term */
			i = term[1] + 2;
			if ( fi == AR.outfile || fi == AS.hidefile ) {
				MesPrint("Ran into precompressed term");
				return(-1);
			}
		}
		else if ( !AR.NoCompress && ( ncomp > 0 ) && AR.sLevel <= 0 ) {	/* Must compress */
			if ( Expressions[AS.CurExpr].newbracketinfo
			&& ( fi == AR.outfile || fi == AS.hidefile ) ) {
				PutBracketInIndex(term,position);
			}
			j = *r++ - 1;
			p = term + 1;
			i--;
			if ( AC.PolyFun ) {
				WORD *polystop, *sa;
				sa = p + i;
				sa -= ABS(sa[-1]);
				polystop = p;
				while ( polystop < sa && *polystop != AC.PolyFun ) {
					polystop += polystop[1];
				}
				while ( i > 0 && j > 0 && *p == *r && p < polystop ) {
					i--; j--; k--; p++; r++;
				}
			}
			else {
				WORD *sa;
				sa = p + i;
				sa -= ABS(sa[-1]);
				while ( i > 0 && j > 0 && *p == *r && p < sa ) { i--; j--; k--; p++; r++; }
			}
			if ( k > -2 ) {
nocompress:
				j = i = *term;
				k = 0;
				p = term;
				r = rr;
				NCOPY(r,p,j);
			}
			else {
				*rr = *term;
				term = p;
				j = i;
				NCOPY(r,p,j);
				j = i;
				i += 2;
				first = 2;
			}
/*					Sabotage getting into the coefficient next time */
			r[-(ABS(r[-1]))] = 0;
			if ( r >= AM.ComprTop ) {
				return(MesPrint("CompressSize of %10l is insufficient",AM.CompressSize));
			}
		}
		else if ( !AR.NoCompress && ( ncomp < 0 ) && AR.sLevel <= 0 ) {
				/* No compress but put in compress buffer anyway */
			if ( Expressions[AS.CurExpr].newbracketinfo
			&& ( fi == AR.outfile || fi == AS.hidefile ) ) {
				PutBracketInIndex(term,position);
			}
			j = *r++ - 1;
			p = term + 1;
			i--;
			if ( AC.PolyFun ) {
				WORD *polystop, *sa;
				sa = p + i;
				sa -= ABS(sa[-1]);
				polystop = p;
				while ( polystop < sa && *polystop != AC.PolyFun ) {
					polystop += polystop[1];
				}
				while ( i > 0 && j > 0 && *p == *r && p < polystop ) {
					i--; j--; k--; p++; r++;
				}
			}
			else {
				while ( i > 0 && j > 0 && *p == *r ) { i--; j--; k--; p++; r++; }
			}
			goto nocompress;
		}
		else if ( AR.sLevel <= 0 && Expressions[AS.CurExpr].newbracketinfo
		&& ( fi == AR.outfile || fi == AS.hidefile ) ) {
			PutBracketInIndex(term,position);
		}
		ret = i;
		ADDPOS(*position,i*sizeof(WORD));
		p = fi->POfill;
		do {
			if ( p >= fi->POstop ) {
#ifdef PARALLEL /* [16mar1998 ar] */
			  if ( fi == AR.outfile && PF.me != MASTER && PF.parallel ){
				PF_BUFFER *sbuf = PF.sbuf;
				sbuf->fill[sbuf->active] = fi->POstop;
				PF_ISendSbuf(MASTER,PF_BUFFER_MSGTAG);
				p = fi->PObuffer = fi->POfill = fi->POfull =
				  sbuf->buff[sbuf->active];
				fi->POstop = sbuf->stop[sbuf->active];
			  }
			  else{
#endif /* PARALLEL [16mar1998 ar] */
				if ( fi->handle < 0 ) {
					if ( ( RetCode = CreateFile(fi->name) ) >= 0 ) {
						fi->handle = (WORD)RetCode;
						PUTZERO(fi->filesize);
					}
					else {
						MesPrint("Cannot create scratch file %s",fi->name);
						return(-1);
					}
				}
				SeekFile(fi->handle,&(fi->POposition),SEEK_SET);
/* --COMPRESS-- */
				if ( WriteFile(fi->handle,(UBYTE *)(fi->PObuffer),fi->POsize) != fi->POsize ) {
					return(MesPrint("Write error during sort. Disk full?"));
				}
				ADDPOS(fi->filesize,fi->POsize);
				p = fi->PObuffer;

				ADDPOS(fi->POposition,fi->POsize);
#ifdef PARALLEL /* [16mar1998 ar] */
			  }
#endif /* PARALLEL [16mar1998 ar] */
			} 
			if ( first ) {
				if ( first == 2 ) *p++ = k;
				else *p++ = j;
				first--;
			}
			else *p++ = *term++;
/*
			if ( AP.DebugFlag ) {
				TalToLine((UWORD)(p[-1])); TokenToLine((UBYTE *)"  ");
			}
*/
		} while ( --i > 0 );
		fi->POfill = p;
	}
/*
	if ( AP.DebugFlag ) {
		AO.OutSkip = 0;
		FiniLine();
	}
*/
	return(ret);
}

/*
 		#] PutOut :
 		#[ FlushOut :				WORD Flushout(position,file)

	Completes output to an output file and writes the trailing zero.

*/

WORD
FlushOut ARG2(POSITION *,position,FILEHANDLE *,fi)
{
  LONG size;
#ifdef PARALLEL /* [16mar1998 ar] */

  if (PF.me != MASTER && fi == AR.outfile && PF.parallel ){
	PF_BUFFER *sbuf = PF.sbuf;
	if ( fi->POfill >= fi->POstop ){
	  sbuf->fill[sbuf->active] = fi->POstop;
	  PF_ISendSbuf(MASTER,PF_BUFFER_MSGTAG);
	  fi->POfull = fi->POfill = fi->PObuffer = sbuf->buff[sbuf->active];
	  fi->POstop = sbuf->stop[sbuf->active];
	}
	*(fi->POfill)++ = 0;
	sbuf->fill[sbuf->active] = fi->POfill;
	PF_ISendSbuf(MASTER,PF_ENDBUFFER_MSGTAG);
	fi->PObuffer = fi->POfill = fi->POfull = sbuf->buff[sbuf->active];
	fi->POstop = sbuf->stop[sbuf->active];
	return(0);
  }
#endif /* PARALLEL [16mar1998 ar] */
	if ( fi->POfill >= fi->POstop ) {
		LONG RetCode;
		if ( fi->handle < 0 ) {
			if ( ( RetCode = CreateFile(fi->name) ) >= 0 ) {
				PUTZERO(fi->filesize);
				fi->handle = (WORD)RetCode;
			}
			else {
				return(MesPrint("Cannot create scratch file %s",fi->name));
			}
		}
		SeekFile(fi->handle,&(fi->POposition),SEEK_SET);
/* --COMPRESS-- */
		if ( WriteFile(fi->handle,(UBYTE *)(fi->PObuffer),fi->POsize) != fi->POsize ) {
			return(MesPrint("Write error while sorting. Disk full?"));
		}
		ADDPOS(fi->filesize,fi->POsize);
		fi->POfill = fi->PObuffer;
		ADDPOS(fi->POposition,fi->POsize);
	} 
	*(fi->POfill)++ = 0;
/*
	{
		UBYTE OutBuf[140];
		if ( AP.DebugFlag ) {
			AO.OutFill = AO.OutputLine = OutBuf;
			AO.OutSkip = 3;
			FiniLine();
			TokenToLine((UBYTE *)"End of expression written");
			FiniLine();
		}
	}
*/
	size = (fi->POfill-fi->PObuffer)*sizeof(WORD);
	if ( fi->handle >= 0 ) {
		SeekFile(fi->handle,&(fi->POposition),SEEK_SET);
/* --COMPRESS-- */
		if ( WriteFile(fi->handle,(UBYTE *)(fi->PObuffer),size) != size ) {
			return(MesPrint("Write error while finishing sorting. Disk full?"));
		}
		ADDPOS(fi->filesize,size);
		ADDPOS(fi->POposition,size);
		fi->POfill = fi->PObuffer;
	}
	if ( AR.sLevel <= 0 && Expressions[AS.CurExpr].newbracketinfo ) {
		BRACKETINFO *b = Expressions[AS.CurExpr].newbracketinfo;
		if ( b->indexfill > 0 ) {
			DIFPOS(b->indexbuffer[b->indexfill-1].next,*position,Expressions[AS.CurExpr].onfile);
		}
	}
	ADDPOS(*position,sizeof(WORD));
  return(0);
}

/*
 		#] FlushOut :
 		#[ AddCoef :				WORD AddCoef(pterm1,pterm2)

		Adds the coefficients of the terms *ps1 and *ps2.
		The problem comes when there is not enough space for a new
		longer coefficient. First a local solution is tried.
		If this is not succesfull we need to move terms around.
		The possibility of a garbage collection should not be
		ignored, as avoiding this costs very much extra space which
		is nearly wasted otherwise.

		If the return value is zero the terms cancelled.

		The resulting term is left in *ps1.
*/

WORD
AddCoef ARG2(WORD **,ps1,WORD **,ps2)
{
	SORTING *S = AR.SS;
	WORD *s1, *s2;
	WORD l1, l2, i;
	WORD OutLen, *t, j;
	UWORD *OutCoef;
	OutCoef = SoScratC;
	s1 = *ps1; s2 = *ps2;
	GETCOEF(s1,l1);
	GETCOEF(s2,l2);
	if ( AddRat((UWORD *)s1,l1,(UWORD *)s2,l2,OutCoef,&OutLen) ) {
		MesCall("AddCoef");
		Terminate(-1);
	}
	if ( AC.ncmod != 0 ) {	/* Note that coefficients are positive! */
		if ( BigLong(OutCoef,OutLen,(UWORD *)AC.cmod,ABS(AC.ncmod)) >= 0 ) {
			SubPLon(OutCoef,OutLen,(UWORD *)AC.cmod,ABS(AC.ncmod),OutCoef,&OutLen);
			OutCoef[OutLen] = 1;
			for ( i = 1; i < OutLen; i++ ) OutCoef[OutLen+i] = 0;
		}
	}
	if ( !OutLen ) { *ps1 = *ps2 = 0; return(0); }
	OutLen <<= 1;
	if ( OutLen < 0 ) i = - ( --OutLen );
	else			  i = ++OutLen;
	if ( l1 < 0 ) l1 = -l1;
	l1 <<= 1; l1++;
	if ( i <= l1 ) {	/* Fits in 1 */
		l1 -= i;
		**ps1 -= l1;
		s2 = (WORD *)OutCoef;
		while ( --i > 0 ) *s1++ = *s2++;
		*s1++ = OutLen;
		while ( --l1 >= 0 ) *s1++ = 0;
		goto RegEnd;
	}
	if ( l2 < 0 ) l2 = -l2;
	l2 <<= 1; l2++;
	if ( i <= l2 ) {	/* Fits in 2 */
		l2 -= i;
		**ps2 -= l2;
		s1 = (WORD *)OutCoef;
		while ( --i > 0 ) *s2++ = *s1++;
		*s2++ = OutLen;
		while ( --l2 >= 0 ) *s2++ = 0;
		*ps1 = *ps2;
		goto RegEnd;
	}

	/* Doesn't fit. Make a new term. */

	t = s1;
	s1 = *ps1;
	j = *s1++ + i - l1;		/* Space needed */
	if ( (S->sFill + j) >= S->sTop2 ) {
		GarbHand();

		s1 = *ps1;
		t = s1 + *s1 - 1;
		j = *s1++ + i - l1;		/* Space needed */
		l1 = *t;
		if ( l1 < 0 ) l1  = - l1;
		t -= l1-1;
	}
	s2 = S->sFill;
	*s2++ = j;
	while ( s1 < t ) *s2++ = *s1++;
	s1 = (WORD *)OutCoef;
	while ( --i > 0 ) *s2++ = *s1++;
	*s2++ = OutLen;
	*ps1 = S->sFill;
	S->sFill = s2;
RegEnd:
	*ps2 = 0;
	if ( **ps1 > AM.MaxTer ) {
		MesPrint("Term to complex after polynomial addition. MaxTermSize = %10l",
		AM.MaxTer);
		Terminate(-1);
	}
	return(1);
}

/*
 		#] AddCoef :
 		#[ AddPoly :				WORD AddPoly(pterm1,pterm2)

		Routine should be called when S->PolyWise != 0. It points then
		to the position of AC.PolyFun in both terms.

		We add the contents of the arguments of the two polynomials.
		Special attention has to be given to special arguments.
		We have to reserve a space equal to the size of one term + the
		size of the argument of the other. The addition has to be done
		in this routine because not all objects are reentrant.
*/

WORD
AddPoly ARG2(WORD **,ps1,WORD **,ps2)
{
	SORTING *S = AR.SS;
	WORD i;
	WORD *s1, *s2, *m, *w, *t;
	s1 = *ps1 + S->PolyWise;
	s2 = *ps2 + S->PolyWise;
	w = AR.WorkPointer;
	if ( w + s1[1] + s2[1] + 12 + ARGHEAD >= AM.WorkTop ) {
		MesPrint("Program was adding polyfun arguments");
		MesWork();
	}
/*
	Add here the two arguments. Is a straight merge.
*/
	AddArgs(s1,s2,w);
/*
	Now we need to store the result in a convenient place.
*/
	if ( w[1] <= FUNHEAD ) { *ps1 = *ps2 = 0; return(0); }
	if ( w[1] <= s1[1] || w[1] <= s2[1] ) {   /* Fits in place. */
		if ( w[1] > s1[1] ) {
			*ps1 = *ps2;
			s1 = s2;
		}
		t = s1 + s1[1];
		m = *ps1 + **ps1;
		i = w[1];
		NCOPY(s1,w,i);
		if ( s1 != t ) {
			while ( t < m ) *s1++ = *t++;
			**ps1 = WORDDIF(s1,(*ps1));
		}
		*ps2 = 0;
	}
	else {		/* Make new term */
#ifdef TESTGARB
		s2 = *ps2;
#endif
		*ps2 = 0;
		if ( (S->sFill + (**ps1 + w[1] - s1[1])) >= S->sTop2 ) {
#ifdef TESTGARB
			MesPrint("------Garbage collection-------");
#endif
			AR.WorkPointer += w[1];
			GarbHand();
			AR.WorkPointer = w;
			s1 = *ps1;
			if ( (S->sFill + (**ps1 + w[1] - s1[1])) >= S->sTop2 ) {
#ifdef TESTGARB
				UBYTE OutBuf[140];
				AO.OutFill = AO.OutputLine = OutBuf;
				AO.OutSkip = 3;
				FiniLine();
				i = *s2;
				while ( --i >= 0 ) {
					TalToLine((UWORD)(*s2++)); TokenToLine((UBYTE *)"  ");
				}
				FiniLine();
				AO.OutFill = AO.OutputLine = OutBuf;
				AO.OutSkip = 3;
				FiniLine();
				s2 = *ps1;
				i = *s2;
				while ( --i >= 0 ) {
					TalToLine((UWORD)(*s2++)); TokenToLine((UBYTE *)"  ");
				}
				FiniLine();
				AO.OutFill = AO.OutputLine = OutBuf;
				AO.OutSkip = 3;
				FiniLine();
				s2 = w;
				i = w[1];
				while ( --i >= 0 ) {
					TalToLine((UWORD)(*s2++)); TokenToLine((UBYTE *)"  ");
				}
				FiniLine();
#endif
				MesPrint("Please increase SmallExtension in %s",setupfilename);
				Terminate(-1);
			}
		}
		t = *ps1;
		s2 = S->sFill;
		m = s2;
		i = S->PolyWise;
		NCOPY(s2,t,i);
		i = w[1];
		NCOPY(s2,w,i);
		t = t + t[1];
		w = *ps1 + **ps1;
		while ( t < w ) *s2++ = *t++;
		*m = WORDDIF(s2,m);
		*ps1 = m;
		S->sFill = s2;
		if ( *m > AM.MaxTer ) {
			MesPrint("Term to complex after polynomial addition. MaxTermSize = %10l",
			AM.MaxTer);
			Terminate(-1);
		}
	}
	return(1);
}

/*
 		#] AddPoly :
 		#[ AddArgs :				VOID AddArgs(arg1,arg2,to)
*/

#define INSLENGTH(x)  w[1] = FUNHEAD+ARGHEAD+x; w[FUNHEAD] = ARGHEAD+x;

VOID
AddArgs ARG3(WORD *,s1,WORD *,s2,WORD *,m)
{
	WORD i1, i2;
	WORD *w = m, *mm, *t, *t1, *t2, *tstop1, *tstop2;
	WORD tempterm[8+FUNHEAD];

	*m++ = AC.PolyFun; *m++ = 0; FILLFUN(m)
	*m++ = 0; *m++ = 0; FILLARG(m)
	if ( s1[FUNHEAD] < 0 || s2[FUNHEAD] < 0 ) {
		if ( s1[FUNHEAD] < 0 ) {
			if ( s2[FUNHEAD] < 0 ) {	/* Both are special */
				if ( s1[FUNHEAD] <= -FUNCTION ) {
					if ( s2[FUNHEAD] == s1[FUNHEAD] ) {
						*m++ = 4+FUNHEAD; *m++ = -s1[FUNHEAD]; *m++ = FUNHEAD;
						FILLFUN(m)
						*m++ = 2; *m++ = 1; *m++ = 3;
						INSLENGTH(4+FUNHEAD)
					}
					else if ( s2[FUNHEAD] <= -FUNCTION ) {
						i1 = functions[-FUNCTION-s1[FUNHEAD]].commute != 0;
						i2 = functions[-FUNCTION-s2[FUNHEAD]].commute != 0;
						if ( ( !i1 && i2 ) || ( i1 == i2 && i1 > i2 ) ) {
							i1 = s2[FUNHEAD];
							s2[FUNHEAD] = s1[FUNHEAD];
							s1[FUNHEAD] = i1;
						}
						*m++ = 4+FUNHEAD; *m++ = -s1[FUNHEAD]; *m++ = FUNHEAD;
						FILLFUN(m)
						*m++ = 1; *m++ = 1; *m++ = 3;
						*m++ = 4+FUNHEAD; *m++ = -s2[FUNHEAD]; *m++ = FUNHEAD;
						FILLFUN(m)
						*m++ = 1; *m++ = 1; *m++ = 3;
						INSLENGTH(8+2*FUNHEAD)
					}
					else if ( s2[FUNHEAD] == -SYMBOL ) {
						*m++ = 8; *m++ = SYMBOL; *m++ = 4; *m++ = s2[FUNHEAD+1]; *m++ = 1;
						*m++ = 1; *m++ = 1; *m++ = 3;
						*m++ = 4+FUNHEAD; *m++ = -s1[FUNHEAD]; *m++ = FUNHEAD;
						FILLFUN(m)
						*m++ = 1; *m++ = 1; *m++ = 3;
						INSLENGTH(12+FUNHEAD)
					}
					else {		/* number */
						*m++ = 4;
						*m++ = ABS(s2[FUNHEAD+1]); *m++ = 1; *m++ = s2[FUNHEAD+1] < 0 ? -3: 3;
						*m++ = 4+FUNHEAD; *m++ = -s1[FUNHEAD]; *m++ = FUNHEAD;
						FILLFUN(m)
						*m++ = 1; *m++ = 1; *m++ = 3;
						INSLENGTH(8+FUNHEAD)
					}
				}
				else if ( s1[FUNHEAD] == -SYMBOL ) {
					if ( s2[FUNHEAD] == s1[FUNHEAD] ) {
						if ( s1[FUNHEAD+1] == s2[FUNHEAD+1] ) {
							*m++ = 8; *m++ = SYMBOL; *m++ = 4; *m++ = s1[FUNHEAD+1];
							*m++ = 1; *m++ = 2; *m++ = 1; *m++ = 3;
							INSLENGTH(8)
						}
						else {
							if ( s1[FUNHEAD+1] > s2[FUNHEAD+1] )
								{ i1 = s2[FUNHEAD+1]; i2 = s1[FUNHEAD+1]; }
							else { i1 = s1[FUNHEAD+1]; i2 = s2[FUNHEAD+1]; }
							*m++ = 8; *m++ = SYMBOL; *m++ = 4; *m++ = i1;
							*m++ = 1; *m++ = 1; *m++ = 1; *m++ = 3;
							*m++ = 8; *m++ = SYMBOL; *m++ = 4; *m++ = i2;
							*m++ = 1; *m++ = 1; *m++ = 1; *m++ = 3;
							INSLENGTH(16)
						}
					}
					else if ( s2[FUNHEAD] <= -FUNCTION ) {
						*m++ = 8; *m++ = SYMBOL; *m++ = 4; *m++ = s1[FUNHEAD+1]; *m++ = 1;
						*m++ = 1; *m++ = 1; *m++ = 3;
						*m++ = 4+FUNHEAD; *m++ = -s2[FUNHEAD]; *m++ = FUNHEAD;
						FILLFUN(m)
						*m++ = 1; *m++ = 1; *m++ = 3;
						INSLENGTH(12+FUNHEAD)
					}
					else {
						*m++ = 4;
						*m++ = ABS(s2[FUNHEAD+1]); *m++ = 1; *m++ = s2[FUNHEAD+1] < 0 ? -3: 3;
						*m++ = 8; *m++ = SYMBOL; *m++ = 4; *m++ = s1[FUNHEAD+1]; *m++ = 1;
						*m++ = 1; *m++ = 1; *m++ = 3;
						INSLENGTH(12)
					}
				}
				else {	/* Must be -SNUMBER! */
					if ( s2[FUNHEAD] <= -FUNCTION ) {
						*m++ = 4;
						*m++ = ABS(s1[FUNHEAD+1]); *m++ = 1; *m++ = s1[FUNHEAD+1] < 0 ? -3: 3;
						*m++ = 4+FUNHEAD; *m++ = -s2[FUNHEAD]; *m++ = FUNHEAD;
						FILLFUN(m)
						*m++ = 1; *m++ = 1; *m++ = 3;
						INSLENGTH(8+FUNHEAD)
					}
					else if ( s2[FUNHEAD] == -SYMBOL ) {
						*m++ = 4;
						*m++ = ABS(s1[FUNHEAD+1]); *m++ = 1; *m++ = s1[FUNHEAD+1] < 0 ? -3: 3;
						*m++ = 8; *m++ = SYMBOL; *m++ = 4; *m++ = s2[FUNHEAD+1]; *m++ = 1;
						*m++ = 1; *m++ = 1; *m++ = 3;
						INSLENGTH(12)
					}
					else {		/* Both are numbers. add. */
						LONG x1;
						x1 = (LONG)s1[FUNHEAD+1] + (LONG)s2[FUNHEAD+1];
						if ( x1 < 0 ) { i1 = (WORD)(-x1); i2 = -3; }
						else { i1 = (WORD)x1; i2 = 3; }
						if ( x1 && AC.ncmod != 0 ) {
							m[0] = 4;
							m[1] = i1;
							m[2] = 1;
							m[3] = i2;
							if ( Modulus(m) ) Terminate(-1);
							if ( *m == 0 ) w[1] = 0;
							else {
								if ( *m == 4 && ( m[1] & MAXPOSITIVE ) == m[1]
								&& m[3] == 3 ) {
									i1 = m[1];
									m -= ARGHEAD;
									*m++ = -SNUMBER;
									*m++ = i1;
									INSLENGTH(4)
								}
								else {
									INSLENGTH(*m)
									m += *m;
								}
							}							
						}
						else {
							if ( x1 == 0 ) {
								w[1] = FUNHEAD;
							}
							else if ( ( i1 & MAXPOSITIVE ) == i1 ) {
								m -= ARGHEAD;
								*m++ = -SNUMBER;
								*m++ = (WORD)x1;
								w[1] = FUNHEAD+2;
							}
							else {
								*m++ = 4; *m++ = i1; *m++ = 1; *m++ = i2;
								INSLENGTH(4)
							}
						}
					}
				}
			}
			else {	/* Only s1 is special */
s1only:
/*
			Compose a term in `tempterm'
*/
				t = tempterm;
				if ( s1[FUNHEAD] <= -FUNCTION ) {
					*t++ = 4+FUNHEAD; *t++ = -s1[FUNHEAD]; *t++ = FUNHEAD;
					FILLFUN(t)
					*t++ = 1; *t++ = 1; *t++ = 3;
				}
				else if ( s1[FUNHEAD] == -SYMBOL ) {
					*t++ = 8; *t++ = SYMBOL; *t++ = 4;
					*t++ = s1[FUNHEAD+1]; *t++ = 1;
					*t++ = 1; *t++ = 1; *t++ = 3;
				}
				else {
					*t++ = 4;   *t++ = ABS(s1[FUNHEAD+1]);
					*t++ = 1;   *t++ = s1[FUNHEAD+1] < 0 ? -3: 3;
				}
				tstop1 = t;
				s1 = tempterm;
				goto twogen;
			}
		}
		else {		/* Only s2 is special */
			t = s1;
			s1 = s2;
			s2 = t;
			goto s1only;
		}
	}
	else {
		tstop1 = s1 + s1[1];
		s1 += FUNHEAD+ARGHEAD;
twogen:
		tstop2 = s2 + s2[1];
		s2 += FUNHEAD+ARGHEAD;
/*
		Now we should merge the expressions in s1 and s2 into m.
*/
		AR.SS->PolyFlag = 0;
		while ( s1 < tstop1 && s2 < tstop2 ) {
			i1 = Compare(s1,s2,(WORD)(-1));
			if ( i1 > 0 ) {
				i2 = *s1;
				NCOPY(m,s1,i2);
			}
			else if ( i1 < 0 ) {
				i2 = *s2;
				NCOPY(m,s2,i2);
			}
			else {	/* Coefficients should be added. */
				WORD i;
				t = s1+*s1;
				i1 = t[-1];
				i2 = *s1 - ABS(i1);
				t2 = s2 + i2;
				s2 += *s2;
				mm = m;
				NCOPY(m,s1,i2);
				t1 = s1;
				s1 = t;
				i2 = s2[-1];
/*
				t1,i1 is the first coefficient
				t2,i2 is the second coefficient
				It should be placed at m,i1
*/
				i1 = REDLENG(i1);
				i2 = REDLENG(i2);
				if ( AddRat((UWORD *)t1,i1,(UWORD *)t2,i2,(UWORD *)m,&i) ) {
					MesPrint("Addition of coefficients of PolyFun");
					Terminate(-1);
				}
				if ( i == 0 ) {
					m = mm;
				}
				else {
					i1 = INCLENG(i);
					m += ABS(i1);
					m[-1] = i1;
					*mm = WORDDIF(m,mm);
					if ( AC.ncmod != 0 ) {
						if ( Modulus(mm) ) Terminate(-1);
						if ( !*mm ) m = mm;
					}
				}
			}
		}
		while ( s1 < tstop1 ) *m++ = *s1++;
		while ( s2 < tstop2 ) *m++ = *s2++;
		w[1] = WORDDIF(m,w);
		w[FUNHEAD] = w[1] - FUNHEAD;
		if ( ToFast(w+FUNHEAD,w+FUNHEAD) ) {
			if ( w[FUNHEAD] <= -FUNCTION ) w[1] = FUNHEAD+1;
			else w[1] = FUNHEAD+2;
			if ( w[FUNHEAD] == -SNUMBER && w[FUNHEAD+1] == 0 ) w[1] = FUNHEAD;
		}
		AR.SS->PolyFlag = 1;
	}
}

/*
 		#] AddArgs :
 		#[ Compare :				WORD Compare(term1,term2,level)

	Compares two terms. The answer is:
	0	equal ( with exception of the coefficient if level == 0. )
	>0	term1 comes first.
	<0	term2 comes first.
	Some special precautions may be needed to keep the CompCoef routine
	from generating overflows, although this is very unlikely in subterms.
	This routine should not return an error condition.
*/

WORD
Compare ARG3(WORD *,term1,WORD *,term2,WORD,level)
{
	SORTING *S = AR.SS;
	WORD *stopper1, *stopper2, *t2;
	WORD *s1, *s2, *t1;
	WORD *stopex1, *stopex2;
	WORD c1, c2;
	WORD prevorder;
	WORD count = -1, localPoly, polyhit = -1;
	
	if ( S->PolyFlag ) {
/*
		if ( S->PolyWise != 0 ) {
			MesPrint("S->PolyWise is not zero!!!!!");
		}
*/
		count = 0; localPoly = 1; S->PolyWise = polyhit = 0;
	}
	else { localPoly = 0; }
	prevorder = 0;
	GETSTOP(term1,s1);
	stopper1 = s1;
	GETSTOP(term2,stopper2);
	t1 = term1 + 1;
	t2 = term2 + 1;
	while ( t1 < stopper1 && t2 < stopper2 ) {
		if ( *t1 != *t2 ) {
			if ( *t1 == HAAKJE ) return(PREV(-1));
			if ( *t2 == HAAKJE ) return(PREV(1));
			if ( *t1 >= (FUNCTION-1) ) {
				if ( *t2 < (FUNCTION-1) ) return(PREV(-1));
				if ( *t1 < FUNCTION && *t2 < FUNCTION ) return(PREV(*t2-*t1));
				if ( *t1 < FUNCTION ) return(PREV(1));
				if ( *t2 < FUNCTION ) return(PREV(-1));
				c1 = functions[*t1-FUNCTION].commute;
				c2 = functions[*t2-FUNCTION].commute;
				if ( !c1 ) {
					if ( c2 ) return(PREV(1));
					else return(PREV(*t2-*t1));
				}
				else {
					if ( !c2 ) return(PREV(-1));
					else return(PREV(*t2-*t1));
				}
			}
			else return(PREV(*t2-*t1));
		}
		s1 = t1 + 2;
		s2 = t2 + 2;
		c1 = *t1;
		t1 += t1[1];
		t2 += t2[1];
		if ( localPoly && c1 < FUNCTION ) {
			polyhit = 1;
		}
		if ( c1 <= (FUNCTION-1)
		|| ( c1 >= FUNCTION && functions[c1-FUNCTION].spec ) ) {
			if ( c1 == SYMBOL ) {
				if ( AC.SortType >= SORTPOWERFIRST ) {
					WORD i1 = 0, *r1;
					r1 = s1;
					while ( s1 < t1 ) { i1 += s1[1]; s1 += 2; }
					s1 = r1; r1 = s2;
					while ( s2 < t2 ) { i1 -= s2[1]; s2 += 2; }
					s2 = r1;
					if ( i1 ) {
						if ( SORTANTIPOWER ) i1 = -i1;
						return(PREV(i1));
					}
				}
				while ( s1 < t1 ) {
					if ( s2 >= t2 ) return(PREV(1));
					if ( *s1 != *s2 ) return(PREV(*s2-*s1));
					s1++; s2++;
					if ( *s1 != *s2 ) return(
						PREV((AC.SortType==SORTLOWFIRST?*s2-*s1:*s1-*s2)));
					s1++; s2++;
				}
			}
			else if ( c1 == DOTPRODUCT ) {
				if ( AC.SortType >= SORTPOWERFIRST ) {
					WORD i1 = 0, *r1;
					r1 = s1;
					while ( s1 < t1 ) { i1 += s1[2]; s1 += 3; }
					s1 = r1; r1 = s2;
					while ( s2 < t2 ) { i1 -= s2[2]; s2 += 3; }
					s2 = r1;
					if ( i1 ) {
						if ( SORTANTIPOWER ) i1 = -i1;
						return(PREV(i1));
					}
				}
				while ( s1 < t1 ) {
					if ( s2 >= t2 ) return(PREV(1));
					if ( *s1 != *s2 ) return(PREV(*s2-*s1));
					s1++; s2++;
					if ( *s1 != *s2 ) return(PREV(*s2-*s1));
					s1++; s2++;
					if ( *s1 != *s2 ) return(
						PREV((AC.SortType==SORTLOWFIRST?*s2-*s1:*s1-*s2)));
					s1++; s2++;
				}
			}
			else {
				while ( s1 < t1 ) {
					if ( s2 >= t2 ) return(PREV(1));
					if ( *s1 != *s2 ) return(PREV(*s2-*s1));
					s1++; s2++;
				}
			}
			if ( s2 < t2 ) return(PREV(-1));
		}
		else {
#if FUNHEAD != 2
			s1 += FUNHEAD-2;
			s2 += FUNHEAD-2;
#endif
			if ( localPoly && c1 == AC.PolyFun ) {
				if ( count == 0 ) {
					WORD i1, i2;
					if ( *s1 > 0 ) i1 = *s1;
					else if ( *s1 <= -FUNCTION ) i1 = 1;
					else i1 = 2;
					if ( *s2 > 0 ) i2 = *s2;
					else if ( *s2 <= -FUNCTION ) i2 = 1;
					else i2 = 2;
					if ( s1+i1 == t1 && s2+i2 == t2 ) {	/* This is the stuff */
/*
						Test for scalar nature?
*/
						if ( !polyhit ) {
							WORD *u1, *u2, *ustop;
							u1 = s1 + ARGHEAD;
							while ( u1 < t1 ) {
								u2 = u1 + *u1;
								ustop = u2 - ABS(u2[-1]);
								u1++;
								while ( u1 < ustop ) {
									if ( *u1 == INDEX ) goto NoPoly;
									u1 += u1[1];
								}
								u1 = u2;
							}
							u1 = s2 + ARGHEAD;
							while ( u1 < t2 ) {
								u2 = u1 + *u1;
								ustop = u2 - ABS(u2[-1]);
								u1++;
								while ( u1 < ustop ) {
									if ( *u1 == INDEX ) goto NoPoly;
									u1 += u1[1];
								}
								u1 = u2;
							}
						}
						S->PolyWise = WORDDIF(s1,term1);
						S->PolyWise -= FUNHEAD;
						count = 1;
						continue;
					}
					else {
NoPoly:
						S->PolyWise = localPoly = 0;
					}
				}
				else {
					t1 = term1 + S->PolyWise;
					t2 = term2 + S->PolyWise;
					S->PolyWise = 0;
					localPoly = 0;
					continue;
				}
			}
			while ( s1 < t1 ) {
/*
				The next statement was added 9-nov-2001. It made a bad error
*/
				if ( s2 >= t2 ) return(PREV(-1));
/*
				There is a little problem here with fast arguments
				We don't want to sacrifice speed, but we like to
				keep a rational ordering. This last one suffers in
				the solution that has been choosen here.
*/
				if ( AC.properorderflag ) {
					if ( ( c2 = -CompArg(s1,s2) ) != 0 ) return(PREV(c2));
					NEXTARG(s1)
					NEXTARG(s2)
				}
				else {
					if ( *s1 > 0 ) {
						if ( *s2 > 0 ) {
							stopex1 = s1 + *s1;
							if ( s2 >= t2 ) return(PREV(-1));
							stopex2 = s2 + *s2;
							s1 += ARGHEAD; s2 += ARGHEAD;
							while ( s1 < stopex1 ) {
								if ( s2 >= stopex2 ) return(PREV(-1));
								if ( ( c2 = Compare(s1,s2,(WORD)1) ) != 0 )
									return(PREV(c2));
								s1 += *s1;
								s2 += *s2;
							}
							if ( s2 < stopex2 ) return(PREV(1));
						}
						else return(PREV(1));
					}
					else {
						if ( *s2 > 0 ) return(PREV(-1));
						if ( *s1 != *s2 ) { return(PREV(*s1-*s2)); }
						if ( *s1 > -FUNCTION ) {
							if ( *++s1 != *++s2 ) { return(PREV(*s2-*s1)); }
						}
						s1++; s2++;
					}
				}
			}
			if ( s2 < t2 ) return(PREV(1));
		}
	}
	{
		if ( AC.SortType != SORTLOWFIRST ) {
			if ( t1 < stopper1 ) return(PREV(1));
			if ( t2 < stopper2 ) return(PREV(-1));
		}
		else {
			if ( t1 < stopper1 ) return(PREV(-1));
			if ( t2 < stopper2 ) return(PREV(1));
		}
	}
	if ( level >= 1 )
		return(CompCoef(term2,term1));
/*	if ( level == 2 ) return(CompCoef(term1,term2)); */
	return(0);
}

/*
 		#] Compare :
 		#[ ComPress :				static LONG ComPress(ss,n)

		Gets a list of pointers to terms and compresses the terms.
		In n it collects the number of terms and the return value
		of the function is the space that is occupied.

		We have to pay some special attention to the compression of
		terms with a PolyFun. This PolyFun should occur only straight
		before the coefficient, so we can use the same trick as for
		the coefficient to sabotage compression of this object
		(Replace in the history the function pointer by zero. This
		is safe, because terms that would be identical otherwise would
		have been added).
*/

static WORD *compressSpace = 0;
static int compressSize = 0;

static LONG ComPress ARG2(WORD **,ss,LONG *,n)
{
	WORD *t, *s, j, k;
	LONG size = 0;
	int newsize, i;
/*
			#[ debug :

	WORD **sss = ss;

	if ( AP.DebugFlag ) {
		UBYTE OutBuf[140];
		MesPrint("ComPress:");
		AO.OutFill = AO.OutputLine = OutBuf;
		AO.OutSkip = 3;
		FiniLine();
		ss = sss;
		while ( *ss ) {
			s = *ss++;
			j = *s;
			if ( j < 0 ) {
				j = s[1] + 2;
			}
			while ( --j >= 0 ) {
				TalToLine((UWORD)(*s++)); TokenToLine((UBYTE *)"  ");
			}
			FiniLine();
		}
		AO.OutSkip = 0;
		FiniLine();
		ss = sss;
	}

			#] debug :
*/
	*n = 0;
	if ( AR.SS == AM.S0 && !AR.NoCompress ) {
		if ( compressSize == 0 ) {
			if ( *ss ) { compressSize = **ss + 64; }
			else       { compressSize = AM.MaxTer + 2; }
			compressSpace = (WORD *)Malloc1(compressSize*sizeof(WORD),"Compression");
		}
		compressSpace[0] = 0;
		while ( *ss ) {
			k = 0;
			s = *ss;
			j = *s++;
			if ( j > compressSize ) {
				newsize = j + 64;
				t = (WORD *)Malloc1(newsize*sizeof(WORD),"Compression");
				if ( compressSpace ) {
					for ( i = 0; i < *compressSpace; i++ ) t[i] = compressSpace[i];
					M_free(compressSpace,"Compression");
				}
				compressSpace = t;
				compressSize = newsize;
			}
			t = compressSpace;
			i = *t - 1;
			*t++ = j; j--;
			if ( AC.PolyFun ) {
				WORD *polystop, *sa;
				sa = s + j;
				sa -= ABS(sa[-1]);
				polystop = s;
				while ( polystop < sa && *polystop != AC.PolyFun ) {
					polystop += polystop[1];
				}
				while ( i > 0 && j > 0 && *s == *t && s < polystop ) {
					i--; j--; s++; t++; k--;
				}
			}
			else {
				WORD *sa;
				sa = s + j;
				sa -= ABS(sa[-1]);
				while ( i > 0 && j > 0 && *s == *t && s < sa ) { i--; j--; s++; t++; k--; }
			}
			if ( k < -1 ) {
				s[-1] = j;
				s[-2] = k;
				*ss = s-2;
				size += j + 2;
			}
			else {
				size += *compressSpace;
				if ( k == -1 ) { t--; s--; j++; }
			}
			while ( --j >= 0 ) *t++ = *s++;
/*					Sabotage getting into the coefficient next time */
			t = compressSpace + *compressSpace;
			t[-(ABS(t[-1]))] = 0;
			ss++;
			(*n)++;
		}
	}
	else {
		while ( *ss ) {
			size += *(*ss++);
			(*n)++;
		}
	}
/*
			#[ debug :

	if ( AP.DebugFlag ) {
		UBYTE OutBuf[140];
		AO.OutFill = AO.OutputLine = OutBuf;
		AO.OutSkip = 3;
		FiniLine();
		ss = sss;
		while ( *ss ) {
			s = *ss++;
			j = *s;
			if ( j < 0 ) {
				j = s[1] + 2;
			}
			while ( --j >= 0 ) {
				TalToLine((UWORD)(*s++)); TokenToLine((UBYTE *)"  ");
			}
			FiniLine();
		}
		AO.OutSkip = 0;
		FiniLine();
	}

			#] debug :
*/
	return(size);
}

/*
 		#] ComPress :
 		#[ SplitMerge :				VOID SplitMerge(Point,number)

		Algorithm by J.A.M.Vermaseren (31-7-1988)

		Note that SplitScratch and InScratch are used also in GarbHand
*/

#ifdef NEWSPLITMERGE

LONG
SplitMerge ARG2(WORD **,Pointer,LONG,number)
{
	SORTING *S = AR.SS;
	WORD **pp3, **pp1, **pp2;
	LONG nleft, nright, i, newleft, newright;
	WORD **pptop;

	if ( number < 2 ) return(number);
	if ( number == 2 ) {
		pp1 = Pointer; pp2 = pp1 + 1;
		if ( ( i = Compare(*pp1,*pp2,(WORD)0) ) < 0 ) {
			pp3 = (WORD **)(*pp1); *pp1 = *pp2; *pp2 = (WORD *)pp3;
		}
		else if ( i == 0 ) {
		  number--;
		  if ( S->PolyWise ) { if ( AddPoly(pp1,pp2) == 0 ) { number = 0; } }
		  else {               if ( AddCoef(pp1,pp2) == 0 ) { number = 0; } }
		}
		return(number);
	}
	pptop = Pointer + number;
	nleft = number >> 1; nright = number - nleft;
	newleft  = SplitMerge(Pointer,nleft);
	newright = SplitMerge(Pointer+nleft,nright);
/*
	We compare the last of the left with the first of the right
	If they are already in order, we will be done quickly.
	We may have to compactify the buffer because the recursion may
	have created holes. Also this compare may result in equal terms.
	Addition of 23-jul-1999. It makes things a bit faster.
*/
	if ( newleft > 0 && newright > 0 &&
	( i = Compare(Pointer[newleft-1],Pointer[nleft],(WORD)0) ) >= 0 ) {
		pp2 = Pointer+nleft; pp1 = Pointer+newleft-1;
		if ( i == 0 ) {
		  if ( S->PolyWise ) {
			if ( AddPoly(pp1,pp2) > 0 ) pp1++;
			else newleft--;
		  }
		  else {               
			if ( AddCoef(pp1,pp2) > 0 ) pp1++;
			else newleft--;
		  }
		  *pp2++ = 0; newright--;
		}
		else pp1++;
		newleft += newright;
		if ( pp1 < pp2 ) {
			while ( --newright >= 0 ) *pp1++ = *pp2++;
			while ( pp1 < pptop ) *pp1++ = 0;
		}
		return(newleft);
	}
	if ( nleft > SplitScratchSize ) {
		SplitScratchSize = (nleft*3)/2+100;
		if ( SplitScratchSize > S->Terms2InSmall/2 )
			 SplitScratchSize = S->Terms2InSmall/2;
		if ( SplitScratch ) M_free(SplitScratch,"SplitScratch");
		SplitScratch = (WORD **)Malloc1(SplitScratchSize*sizeof(WORD *),"SplitScratch");
	}
	pp3 = SplitScratch; pp1 = Pointer; i = nleft;
	do { *pp3++ = *pp1; *pp1++ = 0; } while ( *pp1 && --i > 0 );
	if ( i > 0 ) { *pp3 = 0; i--; }
	InScratch = nleft - i;
	pp1 = SplitScratch; pp2 = Pointer + nleft; pp3 = Pointer;
	while ( *pp1 && *pp2 && nleft > 0 && nright > 0 ) {
		if ( ( i = Compare(*pp1,*pp2,(WORD)0) ) < 0 ) {
			*pp3++ = *pp2;
			*pp2++ = 0;
			nright--;
		}
		else if ( i > 0 ) {
			*pp3++ = *pp1;
			*pp1++ = 0;
			nleft--;
		}
		else {
		  if ( S->PolyWise ) { if ( AddPoly(pp1,pp2) > 0 ) *pp3++ = *pp1; }
		  else {               if ( AddCoef(pp1,pp2) > 0 ) *pp3++ = *pp1; }
		  *pp1++ = 0; *pp2++ = 0; nleft--; nright--;
		}
	}
	while ( *pp1 && --nleft  >= 0 ) { *pp3++ = *pp1; *pp1++ = 0; }
	while ( *pp2 && --nright >= 0 ) { *pp3++ = *pp2++; }
	nleft = pp3 - Pointer;
	while ( pp3 < pptop ) *pp3++ = 0;
	InScratch = 0;
	return(nleft);
}

#else

VOID
SplitMerge ARG2(WORD **,Pointer,LONG,number)
{
	SORTING *S = AR.SS;
	WORD **pp3, **pp1, **pp2;
	LONG nleft, nright, i;
	WORD **pptop;

	if ( number < 2 ) return;
	if ( number == 2 ) {
		pp1 = Pointer; pp2 = pp1 + 1;
		if ( ( i = Compare(*pp1,*pp2,(WORD)0) ) < 0 ) {
			pp3 = (WORD **)(*pp1); *pp1 = *pp2; *pp2 = (WORD *)pp3;
		}
		else if ( i == 0 ) {
		  if ( S->PolyWise ) { if ( !AddPoly(pp1,pp2) ) { *pp1 = 0; } }
		  else {               if ( !AddCoef(pp1,pp2) ) { *pp1 = 0; } }
		  *pp2 = 0;
		}
		return;
	}
	pptop = Pointer + number;
	nleft = number >> 1; nright = number - nleft;
	SplitMerge(Pointer,nleft);
	SplitMerge(Pointer+nleft,nright);
	if ( nleft > SplitScratchSize ) {
		SplitScratchSize = (nleft*3)/2+100;
		if ( SplitScratchSize > S->Terms2InSmall/2 )
			 SplitScratchSize = S->Terms2InSmall/2;
		if ( SplitScratch ) M_free(SplitScratch,"SplitScratch");
		SplitScratch = (WORD **)Malloc1(SplitScratchSize*sizeof(WORD *),"SplitScratch");
	}
	pp3 = SplitScratch; pp1 = Pointer; i = nleft;
	do { *pp3++ = *pp1; *pp1++ = 0; } while ( *pp1 && --i > 0 );
	if ( i > 0 ) { *pp3 = 0; i--; }
	InScratch = nleft - i;
	pp1 = SplitScratch; pp2 = Pointer + nleft; pp3 = Pointer;
	while ( *pp1 && *pp2 && nleft > 0 && nright > 0 ) {
		if ( ( i = Compare(*pp1,*pp2,(WORD)0) ) < 0 ) {
			*pp3++ = *pp2;
			*pp2++ = 0;
			nright--;
		}
		else if ( i > 0 ) {
			*pp3++ = *pp1;
			*pp1++ = 0;
			nleft--;
		}
		else {
		  if ( S->PolyWise ) { if ( AddPoly(pp1,pp2) > 0 ) *pp3++ = *pp1; }
		  else {               
			if ( AddCoef(pp1,pp2) > 0 ) *pp3++ = *pp1; 
		  }
		  *pp1++ = 0; *pp2++ = 0; nleft--; nright--;
		}
	}
	while ( *pp1 && --nleft  >= 0 ) { *pp3++ = *pp1; *pp1++ = 0; }
	while ( *pp2 && --nright >= 0 ) { *pp3++ = *pp2++; }
	while ( pp3 < pptop ) *pp3++ = 0;
	InScratch = 0;

	return;
}

#endif

/*
 		#] SplitMerge :
 		#[ GarbHand :				VOID GarbHand()

		Garbage collection new style. Options:
		1: find some buffer that has enough space (maybe in the large
		   buffer).
		2: allocate a buffer.Give it back afterwards of course.
		The major complication is the buffer for SplitMerge.
*/

VOID
GarbHand()
{
	SORTING *S = AR.SS;
	WORD **Point, *s2, *t, *garbuf, i;
	LONG k, total = 0;
	int tobereturned = 0;
/*
	Compute the size needed. Put it in total.
*/
#ifdef TESTGARB
	MesPrint("in:  S->sFill = %x, S->sTop2 = %x",S->sFill,S->sTop2);
#endif
	Point = S->sPointer;
	k = S->sTerms;
	while ( --k >= 0 ) {
		if ( ( s2 = *Point++ ) != 0 ) { total += *s2; }
	}
	Point = SplitScratch;
	k = InScratch;
	while ( --k >= 0 ) {
		if ( ( s2 = *Point++ ) != 0 ) { total += *s2; }
	}
#ifdef TESTGARB
	MesPrint("total = %l, nterms = %l",2*total,InScratch);
#endif
/*
	Test now whether it fits. If so deal with the problem inside
	the memory at the tail of the large buffer.
*/
	if ( S->lBuffer != 0 && S->lFill + total <= S->lTop ) {
		garbuf = S->lFill;
	}
	else {
		garbuf = (WORD *)Malloc1(total*sizeof(WORD),"Garbage buffer");
		tobereturned = 1;
	}
	t = garbuf;
	Point = S->sPointer;
	k = S->sTerms;
	while ( --k >= 0 ) {
		if ( *Point ) {
			s2 = *Point++;
			i = *s2;
			NCOPY(t,s2,i);
		}
		else { Point++; }
	}
	Point = SplitScratch;
	k = InScratch;
	while ( --k >= 0 ) {
		if ( *Point ) {
			s2 = *Point++;
			i = *s2;
			NCOPY(t,s2,i);
		}
		else Point++;
	}
	s2 = S->sBuffer;
	t = garbuf;
	Point = S->sPointer;
	k = S->sTerms;
	while ( --k >= 0 ) {
		if ( *Point ) {
			*Point++ = s2;
			i = *t;
			NCOPY(s2,t,i);
		}
		else { Point++; }
	}
	Point = SplitScratch;
	k = InScratch;
	while ( --k >= 0 ) {
		if ( *Point ) {
			*Point++ = s2;
			i = *t;
			NCOPY(s2,t,i);
		}
		else Point++;
	}
	S->sFill = s2;
#ifdef TESTGARB
	MesPrint("out: S->sFill = %x, S->sTop2 = %x",S->sFill,S->sTop2);
	if ( S->sFill >= S->sTop2 ) {
		MesPrint("We are in deep trouble");
	}
#endif
	if ( tobereturned ) M_free(garbuf,"Garbage buffer");
	return;
}

/*
 		#] GarbHand :
 		#[ MergePatches :			WORD MergePatches(par)

	The general merge routine. Can be used for the large buffer
	and the file merging. The array S->Patches tells where the patches
	start S->pStop tells where they end (has to be computed first).
	The end of a 'line to be merged' is indicated by a zero. If
	the end is reached without running into a zero or a term
	runs over the boundary of a patch it is a file merging operation
	and a new piece from the file is read in.
	On the Atari one delaying factor could be the omnipresent SeekFile.
	If the FAT is kept in cache memory this operation becomes much
	faster.
	If par == 0 the sort is for file -> outputfile.
	If par == 1 the sort is for large buffer -> sortfile.
	If par == 2 the sort is for large buffer -> outputfile.

*/

#include <stdio.h>

WORD
MergePatches ARG1(WORD,par)
{
	SORTING *S = AR.SS;
	WORD **poin, **poin2, ul, k, i, im, *m1;
	WORD *p, lpat, mpat, level, l1, l2, r1, r2, r3, c;
	WORD *m2, *m3, r31, r33, ki, inNum, *rr;
	UWORD *coef;
	POSITION position;
	POSITION *inPatch = 0;
	FILEHANDLE *fin, *fout;
	int fhandle;
/*
	UBYTE *s;
*/
	if ( AM.safetyfirst != 1 ) return(0);
	fin = &S->file;
	fout = &(AR.FoStage4[0]);
	S->PolyFlag = AC.PolyFun ? 1: 0;
NewMerge:
	coef = SoScratC;
	poin = S->poina; poin2 = S->poin2a;
	rr = AR.CompressPointer;
	*rr = 0;
/*
 		#[ Setup :
*/
	if ( par == 1 ) {
		fout = &(S->file);
		if ( fout->handle < 0 ) {
FileMake:
			PUTZERO(AR.OldPosOut);
			if ( ( fhandle = CreateFile(fout->name) ) < 0 )
				return(MesPrint("Cannot create file %s",fout->name));
			fout->handle = fhandle;
			PUTZERO(fout->filesize);
			SeekFile(fout->handle,&(fout->filesize),SEEK_SET);
			S->fPatchN = 0;
			PUTZERO(S->fPatches[0]);
			fout->POfill = fout->PObuffer;	
			PUTZERO(fout->POposition);
		}
ConMer:
		StageSort(fout);
	}
	else if ( par == 0 && S->stage4 > 0 ) {
/*
		We will have to do our job more than once.
		Input is from S->file and output will go to AR.FoStage4.
		The file corresponding to this last one must be made now.
*/
		AR.Stage4Name ^= 1;
/*
		s = (UBYTE *)(fout->name); while ( *s ) s++;
		if ( AR.Stage4Name ) s[-1] += 1;
		else                s[-1] -= 1;
*/
		inPatch = S->fPatches;
		S->fPatches = S->inPatches;
		S->inPatches = inPatch;
		inNum = S->fPatchN;
		AR.OldPosIn = AR.OldPosOut;
		S->stage4 = 0;
		goto FileMake;
	}
	else {
		if ( par == 0 ) {
			inPatch = S->fPatches;
			inNum = S->fPatchN;
		}
		fout = AR.outfile;
	}
	if ( par ) {				/* Mark end of patches */
		S->Patches[S->lPatch] = S->lFill;
		for ( i = 0; i < S->lPatch; i++ ) {
			S->pStop[i] = S->Patches[i+1]-1;
			S->Patches[i] += AM.MaxTer;
/*
			{
				WORD jtel = 0;
				WORD *ttel = S->Patches[i];
				printf("The contents are for i = %d\n\n",i);
				while ( ttel < S->pStop[i] ) {
					jtel++;
					printf("%d ",*ttel);
					ttel++;
					if ( jtel >= 20 ) { printf("\n"); jtel = 0; }
				}
				printf("\n");
			}
*/
		}
	}
	else {	/* Load the patches */
		S->lPatch = inNum;
#ifdef PARALLEL /* [16aug1998 ar] */
	if ( S->lPatch > 1 || fout == AR.outfile) {
#else
	if ( S->lPatch > 1 ) {
#endif /* PARALLEL [16aug1998 ar] */
			p = S->lBuffer;
			for ( i = 0; i < S->lPatch; i++ ) {
				p += AM.MaxTer + COMPINC;
				S->Patches[i] = p;
				p = (WORD *)(((UBYTE *)p) + fin->POsize);
				S->pStop[i] = m2 = p;
				ADDPOS(inPatch[i],PutIn(fin,&(inPatch[i]),S->Patches[i],&m2));
			}
		}
	}
	if ( fout->handle >= 0 ) {
		PUTZERO(position);
/*		TELLFILE(fout->handle,&position); */
/*		SeekFile(fout->handle,&position,SEEK_CUR); */
		SeekFile(fout->handle,&position,SEEK_END);
		ADDPOS(position,((fout->POfill-fout->PObuffer)*sizeof(WORD)));
	}
	else {
		SETBASEPOSITION(position,(fout->POfill-fout->PObuffer)*sizeof(WORD));
	}
/*
 		#] Setup :

	The following code will have to be eliminated because all output
	should go through PutOut to facilitate set numbers
*/
	if ( S->lPatch == 1 ) {	/* Single patch --> direct copy. Very rare. */
		LONG length;

		if ( fout->handle < 0 ) if ( Sflush(fout) ) goto PatCall;
		if ( par ) {		/* Memory to file */
			length = (LONG)(*S->pStop)-(LONG)(*S->Patches)+sizeof(WORD);
/* --COMPRESS-- */
			if ( WriteFile(fout->handle,(UBYTE *)(*S->Patches),length) != length )
				goto PatwCall;
			ADDPOS(position,length);
			ADDPOS(fout->POposition,length);
			ADDPOS(fout->filesize,length);
			ADDPOS(S->SizeInFile[par],length/sizeof(WORD));
		}
		else {				/* File to file */
			SeekFile(fin->handle,&(inPatch[0]),SEEK_SET); /* needed for stage4 */
/* --COMPRESS-- */
			while ( ( length = ReadFile(fin->handle,
					(UBYTE *)(S->sBuffer),S->SmallEsize*sizeof(WORD)) ) > 0 )
			{
/* --COMPRESS-- */
				if ( WriteFile(fout->handle,(UBYTE *)(S->sBuffer),length) != length )
					goto PatwCall;
				ADDPOS(position,length);
				ADDPOS(fout->POposition,length);
				ADDPOS(fout->filesize,length);
				ADDPOS(S->SizeInFile[par],length/sizeof(WORD));
			}
			if ( length < 0 ) { MesPrint("Readerror"); goto PatCall; }
		}
		goto EndOfAll;
	}
	else if ( S->lPatch > 0 ) {

		/* More than one patch. Construct the tree. */

		lpat = 1;
		do { lpat <<= 1; } while ( lpat < S->lPatch );
		mpat = ( lpat >> 1 ) - 1;
		k = lpat - S->lPatch;

		/* k is the number of empty places in the tree. they will
		   be at the even positions from 2 to 2*k */

		for ( i = 1; i < lpat; i++ ) S->tree[i] = -1;
		for ( i = 1; i <= k; i++ ) {
			im = ( i << 1 ) - 1;
			poin[im] = S->Patches[i-1];
			poin2[im] = poin[im] + *(poin[im]);
			S->used[i] = im;
			S->ktoi[im] = i-1;
			S->tree[mpat+i] = 0;
			poin[im-1] = poin2[im-1] = 0;
		}
		for ( i = (k<<1)+1; i <= lpat; i++ ) {
			S->used[i-k] = i;
			S->ktoi[i] = i-k-1;
			poin[i] = S->Patches[i-k-1];
			poin2[i] = poin[i] + *(poin[i]);
		}
/*
		the array poin tells the position of the i-th element of the S->tree
		'S->used' is a stack with the S->tree elements that need to be entered
		into the S->tree. at the beginning this is S->lPatch. during the
		sort there will be only very few elements.
		poin2 is the next value of poin. it has to be determined
		before the comparisons as the position or the size of the
		term indicated by poin may change.
		S->ktoi translates a S->tree element back to its stream number.

		start the sort
*/
		level = S->lPatch;

		/* introduce one term */
OneTerm:
		k = S->used[level];
		i = k + lpat - 1;
		if ( !*(poin[k]) ) {
			do { if ( !( i >>= 1 ) ) goto EndOfMerge; } while ( !S->tree[i] );
			if ( S->tree[i] == -1 ) {
				S->tree[i] = 0;
				level--;
				goto OneTerm;
			}
			k = S->tree[i];
			S->used[level] = k;
			S->tree[i] = 0;
		}
/*
		move terms down the tree
*/
		while ( i >>= 1 ) {
			if ( S->tree[i] > 0 ) {
				if ( ( c = Compare(poin[S->tree[i]],poin[k],(WORD)0) ) > 0 ) {
/*
					S->tree[i] is the smaller. Exchange and go on.
*/
					S->used[level] = S->tree[i];
					S->tree[i] = k;
					k = S->used[level];
				}
				else if ( !c ) {	/* Terms are equal */
					S->TermsLeft--;
/*
						Here the terms are equal and their coefficients
						have to be added.
*/
					l1 = *( m1 = poin[S->tree[i]] );
					l2 = *( m2 = poin[k] );
					if ( S->PolyWise ) {  /* Here we work with PolyFun */
						WORD *tt1, *w;
						tt1 = m1;
						m1 += S->PolyWise;
						m2 += S->PolyWise;
						w = AR.WorkPointer;
						if ( w + m1[1] + m2[1] > AM.WorkTop ) {
							MesPrint("A WorkSpace of %10l is too small",AM.WorkSize);
							Terminate(-1);
						}
						AddArgs(m1,m2,w);
						r1 = w[1];
						if ( r1 <= FUNHEAD ) { goto cancelled; }
						if ( r1 == m1[1] ) {
							NCOPY(m1,w,r1);
						}
						else if ( r1 < m1[1] ) {
							r2 = m1[1] - r1;
							m2 = w + r1;
							m1 += m1[1];
							while ( --r1 >= 0 ) *--m1 = *--m2;
							m2 = m1 - r2;
							r1 = S->PolyWise;
							while ( --r1 >= 0 ) *--m1 = *--m2;
							*m1 -= r2;
							poin[S->tree[i]] = m1;
						}
						else {
							r2 = r1 - m1[1];
							m2 = tt1 - r2;
							r1 = S->PolyWise;
							m1 = tt1;
							*m1 += r2;
							poin[S->tree[i]] = m2;
							NCOPY(m2,m1,r1);
							r1 = w[1];
							NCOPY(m2,w,r1);
						}
					}
					else {
					  r1 = *( m1 += l1 - 1 );
					  m1 -= ABS(r1) - 1;
					  r1 = ( ( r1 > 0 ) ? (r1-1) : (r1+1) ) >> 1;
					  r2 = *( m2 += l2 - 1 );
					  m2 -= ABS(r2) - 1;
					  r2 = ( ( r2 > 0 ) ? (r2-1) : (r2+1) ) >> 1;

					  if ( AddRat((UWORD *)m1,r1,(UWORD *)m2,r2,coef,&r3) ) {
						MesCall("MergePatches");
						SETERROR(-1)
					  }

					  if ( AC.ncmod != 0 ) {
						if ( BigLong(coef,r3,(UWORD *)AC.cmod,ABS(AC.ncmod)) >= 0 ) {
							WORD ii;
							SubPLon(coef,r3,(UWORD *)AC.cmod,ABS(AC.ncmod),coef,&r3);
							coef[r3] = 1;
							for ( ii = 1; ii < r3; ii++ ) coef[r3+ii] = 0;
						}
					  }
					  r3 <<= 1;
					  r33 = ( r3 > 0 ) ? ( r3 + 1 ) : ( r3 - 1 );
					  if ( r3 < 0 ) r3 = -r3;
					  if ( r1 < 0 ) r1 = -r1;
					  r1 <<= 1;
					  r31 = r3 - r1;
					  if ( !r3 ) {		/* Terms cancel */
cancelled:
						ul = S->used[level] = S->tree[i];
						S->tree[i] = -1;
/*
						We skip to the next term in stream ul
*/
						im = *poin2[ul];
						if ( im < 0 ) {
							r1 = poin2[ul][1] - im + 1;
							m1 = poin2[ul] + 2;
							m2 = poin[ul] - im + 1;
							while ( ++im <= 0 ) *--m1 = *--m2;
							*--m1 = r1;
							poin2[ul] = m1;
							im = r1;
						}
						poin[ul] = poin2[ul];
						ki = S->ktoi[ul];
						if ( !par && (poin[ul] + im + COMPINC) >= S->pStop[ki]
						&& im > 0 ) {
							ADDPOS(inPatch[ki],PutIn(fin,&(inPatch[ki]),
							S->Patches[ki],&(poin[ul])));
							poin2[ul] = poin[ul] + im;
						}
						else {
							poin2[ul] += im;
						}
						S->used[++level] = k;
						S->TermsLeft--;
					  }
					  else if ( !r31 ) {		/* copy coef into term1 */
						goto CopCof2;
					  }
					  else if ( r31 < 0 ) {		/* copy coef into term1
											and adjust the length of term1 */
						goto CopCoef;
					  }
					  else {
/*
							this is the dreaded calamity.
							is there enough space?
*/
						if( (poin[S->tree[i]]+l1+r31) >= poin2[S->tree[i]] ) {
/*
								no space! now the special trick for which
								we left 2*maxlng spaces open at the beginning
								of each patch.
*/
							if ( (l1 + r31) > AM.MaxTer ) {
								return(MesPrint("Coefficient overflow during sort"));
							}
							m2 = poin[S->tree[i]];
							m3 = ( poin[S->tree[i]] -= r31 );
							do { *m3++ = *m2++; } while ( m2 < m1 );
							m1 = m3;
						}
CopCoef:
						*(poin[S->tree[i]]) += r31;
CopCof2:
						m2 = (WORD *)coef; im = r3;
						NCOPY(m1,m2,im);
						*m1 = r33;
					  }
					}
/*
					Now skip to the next term in stream k.
*/
NextTerm:
					im = poin2[k][0];
					if ( im < 0 ) {
						r1 = poin2[k][1] - im + 1;
						m1 = poin2[k] + 2;
						m2 = poin[k] - im + 1;
						while ( ++im <= 0 ) *--m1 = *--m2;
						*--m1 = r1;
						poin2[k] = m1;
						im = r1;
					}
					poin[k] = poin2[k];
					ki = S->ktoi[k];
					if ( !par && ( (poin[k] + im + COMPINC) >= S->pStop[ki] )
					&& im > 0 ) {
						ADDPOS(inPatch[ki],PutIn(fin,&(inPatch[ki]),
						S->Patches[ki],&(poin[k])));
						poin2[k] = poin[k] + im;
					}
					else {
						poin2[k] += im;
					}
					goto OneTerm;
				}
			}
			else if ( S->tree[i] < 0 ) {
				S->tree[i] = k;
				level--;
				goto OneTerm;
			}
		}
/*
			found the smallest in the set. indicated by k.
			write to its destination.
*/
		if ( ( im = PutOut(poin[k],&position,fout,1) ) < 0 ) return(-1);
		ADDPOS(S->SizeInFile[par],im);
		goto NextTerm;
	}
	else {
		return(0);
	}
EndOfMerge:
	if ( FlushOut(&position,fout) ) return(-1);
	ADDPOS(S->SizeInFile[par],1);
EndOfAll:
	if ( par == 1 ) {	/* Set the fpatch pointers */
		(S->fPatchN)++;
		S->fPatches[S->fPatchN] = position;
	}
	if ( par == 0 && fout != AR.outfile ) {
/*
			Output went to sortfile. We have two possibilities:
			1:	We are not finished with the current in-out cycle
				In that case we should pop to the next set of patches
			2:	We finished a cycle and should clean up the in file
				Then we restart the sort.
*/
		(S->fPatchN)++;
		S->fPatches[S->fPatchN] = position;
		if ( ISNOTZEROPOS(AR.OldPosIn) ) {		/* We are not done */
			SeekFile(fin->handle,&(AR.OldPosIn),SEEK_SET);
/* --COMPRESS-- */
			if ( (ULONG)ReadFile(fin->handle,(UBYTE *)(&inNum),(LONG)sizeof(WORD)) !=
				sizeof(WORD)
			  || (ULONG)ReadFile(fin->handle,(UBYTE *)(&AR.OldPosIn),(LONG)sizeof(POSITION)) !=
				sizeof(POSITION)
			  || (ULONG)ReadFile(fin->handle,(UBYTE *)inPatch,(LONG)(inNum+1)
					*sizeof(POSITION)) != (inNum+1)*sizeof(POSITION) ) {
				return(MesPrint("Read error fourth stage sorting"));
			}
			*rr = 0;
			goto ConMer;
		}
		else {
/*
			if ( fin == &(AR.FoStage4[0]) ) {
				s = (UBYTE *)(fin->name); while ( *s ) s++;
				if ( AR.Stage4Name == 1 ) s[-1] -= 1;
				else                     s[-1] += 1;
			}
*/
			CloseFile(fin->handle);
			remove(fin->name);		/* Gives diskspace free again. */
/*
			if ( fin == &(AR.FoStage4[0]) ) {
				s = (UBYTE *)(fin->name); while ( *s ) s++;
				if ( AR.Stage4Name == 1 ) s[-1] += 1;
				else                     s[-1] -= 1;
			}
*/
			fin->handle = -1;
			{ FILEHANDLE *ff = fin; fin = fout; fout = ff; }
			PUTZERO(S->SizeInFile[0]);
			goto NewMerge;
		}
	}
	if ( par == 0 ) {
		CloseFile(fin->handle);
		remove(fin->name);
		fin->handle = -1;
	}
	return(0);

PatwCall:
	MesPrint("Error while writing to file.");
PatCall:
	MesCall("MergePatches");
	SETERROR(-1)
}

/*
 		#] MergePatches :
 		#[ StoreTerm :				WORD StoreTerm(term)

	The central routine to accept terms, store them and keep things
	at least partially sorted. A call to EndSort will then complete
	storing and sorting.
*/
WORD
StoreTerm ARG1(WORD *,term)
{
	SORTING *S = AR.SS;
	WORD **ss, *lfill, j, *t;
	POSITION pp;
	LONG lSpace, sSpace, RetCode, over, tover;

	if ( ( AC.PreDebug & DUMPTOSORT ) == DUMPTOSORT ) PrintTerm(term,"StoreTerm");
	if ( AR.exitflag && AR.sLevel == 0 ) return(0);
  S->sFill = *(S->PoinFill);
  if ( S->sTerms >= S->TermsInSmall || ( S->sFill + *term ) >= S->sTop ) {
/*
	The small buffer is full. It has to be sorted and written.
*/
		S->PolyFlag = ( AC.PolyFun != 0 ) ? 1:0;
		tover = over = S->sTerms;
		ss = S->sPointer;
		ss[over] = 0;
/*
		PrintTime();
*/
		SplitMerge(ss,over);
		sSpace = 0;
		if ( over > 0 ) {
			ss[over] = 0;
			sSpace = ComPress(ss,&RetCode);
			S->TermsLeft -= over - RetCode;
		}
		sSpace++;

		lSpace = sSpace + (S->lFill - S->lBuffer) - AM.MaxTer*((LONG)S->lPatch);
		SETBASEPOSITION(pp,lSpace);
		MULPOS(pp,sizeof(WORD));
		if ( S->file.handle >= 0 ) {
			ADD2POS(pp,S->fPatches[S->fPatchN]);
		}
		if ( S == AM.S0 ) {	/* Only statistics at ground level */
			WORD oldLogHandle = AC.LogHandle;
			if ( AC.LogHandle >= 0 && AM.LogType ) AC.LogHandle = -1;
			WriteStats(&pp,(WORD)0);
			AC.LogHandle = oldLogHandle;
		}
/*
		if ( !S->lPatch && ( S->lFill + sSpace + 2*AM.MaxTer ) >= S->lTop )
			goto SkipLarge;
*/
		if ( ( S->lPatch >= S->MaxPatches ) ||
/*            Albert had here       2*AM.MaxTal     */
			( ( S->lFill + sSpace + 2*AM.MaxTer ) >= S->lTop ) ) {
/*
			The large buffer is too full. Merge and write it
*/
			if ( MergePatches(1) ) goto StoreCall;
			pp = S->SizeInFile[1];
			ADDPOS(pp,sSpace);
			MULPOS(pp,sizeof(WORD));
			if ( S == AM.S0 ) {	/* Only statistics at ground level */
				WORD oldLogHandle = AC.LogHandle;
				if ( AC.LogHandle >= 0 && AM.LogType ) AC.LogHandle = -1;
				WriteStats(&pp,(WORD)1);
				AC.LogHandle = oldLogHandle;
			}
			S->lPatch = 0;
			S->lFill = S->lBuffer;
		}
		S->Patches[S->lPatch++] = S->lFill;
		lfill = S->lFill + AM.MaxTer;
		if ( tover > 0 ) {
			ss = S->sPointer;
			while ( ( t = *ss++ ) != 0 ) {
				j = *t;
				if ( j < 0 ) j = t[1] + 2;
				while ( --j >= 0 ){
				  *lfill++ = *t++;
				}
			}
		}
		*lfill++ = 0;
		S->lFill = lfill;
		S->sTerms = 0;
		S->PoinFill = S->sPointer;
		*(S->PoinFill) = S->sFill = S->sBuffer;
	}
	j = *term;
	while ( --j >= 0 ) *S->sFill++ = *term++;
	S->sTerms++;
	S->GenTerms++;
	S->TermsLeft++;
	*++S->PoinFill = S->sFill;

	return(0);

StoreCall:
	MesCall("StoreTerm");
	SETERROR(-1)
}

/*
 		#] StoreTerm :
 		#[ StageSort :				VOID StageSort(FILEHANDLE *fout)
*/

static VOID
StageSort ARG1(FILEHANDLE *,fout)
{
	SORTING *S = AR.SS;
	if ( S->fPatchN >= S->MaxFpatches ) {
		POSITION position;
		PUTZERO(position);
	MesPrint("StageSort");
		SeekFile(fout->handle,&position,SEEK_END);
/* --COMPRESS-- */
		if ( (ULONG)WriteFile(fout->handle,(UBYTE *)(&(S->fPatchN)),(LONG)sizeof(WORD)) !=
			sizeof(WORD)
		  || (ULONG)WriteFile(fout->handle,(UBYTE *)(&(AR.OldPosOut)),(LONG)sizeof(POSITION)) !=
			sizeof(POSITION)
		  || (ULONG)WriteFile(fout->handle,(UBYTE *)(S->fPatches),(LONG)(S->fPatchN+1)
					*sizeof(POSITION)) != (S->fPatchN+1)*sizeof(POSITION) ) {
			MesPrint("Write error while staging sort. Disk full?");
			Terminate(-1);
		}
		AR.OldPosOut = position;
		fout->filesize = position;
		ADDPOS(fout->filesize,(S->fPatchN+2)*sizeof(POSITION) + sizeof(WORD));
		fout->POposition = fout->filesize;
		S->fPatches[0] = fout->filesize;
		S->fPatchN = 0;

		if ( AR.FoStage4[0].PObuffer == 0 ) {
			AR.FoStage4[0].PObuffer = (WORD *)Malloc1(AR.FoStage4[0].POsize*sizeof(WORD)
												,"Stage 4 buffer");
			AR.FoStage4[0].POfill   = AR.FoStage4[0].PObuffer;
			AR.FoStage4[0].POstop   = AR.FoStage4[0].PObuffer
						 + AR.FoStage4[0].POsize/sizeof(WORD);
		}
		if ( AR.FoStage4[1].PObuffer == 0 ) {
			AR.FoStage4[1].PObuffer = (WORD *)Malloc1(AR.FoStage4[1].POsize*sizeof(WORD)
												,"Stage 4 buffer");
			AR.FoStage4[1].POfill   = AR.FoStage4[1].PObuffer;
			AR.FoStage4[1].POstop   = AR.FoStage4[1].PObuffer
						 + AR.FoStage4[1].POsize/sizeof(WORD);
		}
		S->stage4 = 1;
	}
}

/*
 		#] StageSort :
 		#[ SortWild :				WORD SortWild(w,nw)

	Sorts the wildcard entries in the parameter w. Double entries
	are removed. Full space taken is nw words.
	Routine serves for the reading of wildcards in the compiler.
	The entries come in the format:
	(type,4,number,0) in which the zero is reserved for the
	future replacement of 'number'.

*/

WORD
SortWild ARG2(WORD *,w,WORD,nw)
{
	WORD *v, *s, *m, k, i;
	WORD *pScrat, *stop, *sv, error = 0;
	pScrat = AR.WorkPointer;
	if ( ( AR.WorkPointer + 8 * AM.MaxWildcards ) >= AM.WorkTop ) {
		return(MesWork());
	}
	stop = w + nw;
	i = 0;
	while ( i < nw ) {
		m = w + i;
		v = m + m[1];
		while ( v < stop && (
			 *v == FROMSET || *v == SETTONUM || *v == LOADDOLLAR ) ) v += v[1];
		while ( v < stop ) {
			if ( *v >= 0 ) {
				if ( AM.Ordering[*v] < AM.Ordering[*m] ) {
					m = v;
				}
				else if ( *v == *m ) {
					if ( v[2] < m[2] ) {
						m = v;
					}
					else if ( v[2] == m[2] ) {
						s = m + m[1];
						sv = v + v[1];
						if ( s < stop && ( *s == FROMSET
						 || *s == SETTONUM || *s == LOADDOLLAR ) ) {
							if ( sv < stop && ( *sv == FROMSET
							|| *sv == SETTONUM || *sv == LOADDOLLAR ) ) {
								if ( s[2] != sv[2] ) {
									error = -1;
									MesPrint("&Wildcard set conflict");
								}
							}
							*v = -1;
						}
						else {
							if ( sv < stop && ( *sv == FROMSET
							|| *sv == SETTONUM || *sv == LOADDOLLAR ) ) {
								*m = -1;
								m = v;
							}
							else {
								*v = -1;
							}
						}
					}
				}
			}
			v += v[1];
			while ( v < stop && ( *v == FROMSET
			 || *v == SETTONUM || *v == LOADDOLLAR ) ) v += v[1];
		}
		s = pScrat;
		v = m;
		k = m[1];
		NCOPY(s,m,k);
		while ( m < stop && ( *m == FROMSET
		 || *m == SETTONUM || *m == LOADDOLLAR ) ) {
			k = m[1];
			NCOPY(s,m,k);
		}
		*v = -1;
		pScrat = s;
		i = 0;
		while ( i < nw && ( w[i] < 0 || w[i] == FROMSET
		|| w[i] == SETTONUM || w[i] == LOADDOLLAR ) ) i += w[i+1];
	}
	AC.NwildC = k = WORDDIF(pScrat,AR.WorkPointer);
	s = AR.WorkPointer;
	m = w;
	NCOPY(m,s,k);
	AC.WildC = m;
	return(error);
}

/*
 		#] SortWild :
 		#[ CleanUpSort :			VOID CleanUpSort(num)

		Partially or completely frees function sort buffers.
*/

void CleanUpSort ARG1(int,num)
{
	SORTING *S;
	int minnum = num, i;
	if ( FunSorts ) {
		if ( num == -1 ) {
			if ( MaxFunSorts > 3 ) {
				minnum = (MaxFunSorts+4)/2;
			}
			else minnum = 4;
		}
		else if ( minnum == 0 ) minnum = 1;
		for ( i = minnum; i < NumFunSorts; i++ ) {
			S = FunSorts[i];
			if ( S ) {
				if ( S->file.handle >= 0 ) {
					CloseFile(S->file.handle);
					S->file.handle = -1;
					remove(S->file.name);
				}
				M_free(S,"sorting struct");
			}
			FunSorts[i] = 0;
		}
		if ( num == 0 ) {
			S = FunSorts[0];
			if ( S ) {
				if ( S->file.handle >= 0 ) {
					CloseFile(S->file.handle);
					S->file.handle = -1;
					remove(S->file.name);
				}
			}
		}
	}
	for ( i = 0; i < 2; i++ ) {
		if ( AR.FoStage4[i].handle >= 0 ) {
			CloseFile(AR.FoStage4[i].handle);
			remove(AR.FoStage4[i].name);
			AR.FoStage4[i].handle = -1;
		}
	}
}

/*
 		#] CleanUpSort :
 		#[ LowerSortLevel :         VOID LowerSortLevel()
*/

VOID LowerSortLevel ARG0
{
	if ( AR.sLevel >= 0 ) {
		AR.sLevel--;
		if ( AR.sLevel >= 0 ) AR.SS = FunSorts[AR.sLevel];
	}
}

/*
 		#] LowerSortLevel :
	#] SortUtilities :
*/
