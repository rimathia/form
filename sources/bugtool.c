/** @file bugtool.c
 * 
 *  Low level routines for debugging
 */

/* #[ License : */
/*
 *   Copyright (C) 1984-2013 J.A.M. Vermaseren
 *   When using this file you are requested to refer to the publication
 *   J.A.M.Vermaseren "New features of FORM" math-ph/0010025
 *   This is considered a matter of courtesy as the development was paid
 *   for by FOM the Dutch physics granting agency and we would like to
 *   be able to track its scientific use to convince FOM of its value
 *   for the community.
 *
 *   This file is part of FORM.
 *
 *   FORM is free software: you can redistribute it and/or modify it under the
 *   terms of the GNU General Public License as published by the Free Software
 *   Foundation, either version 3 of the License, or (at your option) any later
 *   version.
 *
 *   FORM is distributed in the hope that it will be useful, but WITHOUT ANY
 *   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *   details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with FORM.  If not, see <http://www.gnu.org/licenses/>.
 */
/* #] License : */ 

/*
  	#[ Includes :
*/

#include "form3.h"
 
/*
  	#] Includes : 
  	#[ ExprStatus :
*/

static UBYTE *statusexpr[] = {
	 (UBYTE *)"LOCALEXPRESSION"
	,(UBYTE *)"SKIPLEXPRESSION"
	,(UBYTE *)"DROPLEXPRESSION"
	,(UBYTE *)"DROPPEDEXPRESSION"
	,(UBYTE *)"GLOBALEXPRESSION"
	,(UBYTE *)"SKIPGEXPRESSION"
	,(UBYTE *)"DROPGEXPRESSION"
	,(UBYTE *)"UNKNOWN"
	,(UBYTE *)"STOREDEXPRESSION"
	,(UBYTE *)"HIDDENLEXPRESSION"
	,(UBYTE *)"HIDELEXPRESSION"
	,(UBYTE *)"DROPHLEXPRESSION"
	,(UBYTE *)"UNHIDELEXPRESSION"
	,(UBYTE *)"HIDDENGEXPRESSION"
	,(UBYTE *)"HIDEGEXPRESSION"
	,(UBYTE *)"DROPHGEXPRESSION"
	,(UBYTE *)"UNHIDEGEXPRESSION"
	,(UBYTE *)"INTOHIDELEXPRESSION"
	,(UBYTE *)"INTOHIDEGEXPRESSION"
};

void ExprStatus(EXPRESSIONS e)
{
	MesPrint("Expression %s(%d) has status %s(%d,%d). Buffer: %d, Position: %15p",
		AC.exprnames->namebuffer+e->name,(WORD)(e-Expressions),
		statusexpr[e->status],e->status,e->hidelevel,
		e->whichbuffer,&(e->onfile));
}

/*
  	#] ExprStatus : 
*/
