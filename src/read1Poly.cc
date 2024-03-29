/* ---------------------------------------------------------------
  RCALI R package
  Copyright INRA 2017
  INRA, UR1404, Research Unit MaIAGE
  F78352 Jouy-en-Josas, France.
 
  URL: http://genome.jouy.inra.fr/logiciels/RCALI
 
  This file is part of RCALI R package.
  RCALI is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  See the GNU General Public License at:
  http://www.gnu.org/licenses/
 
-------------------------------------------------------------- */


/*--------------- IDENTIFICATION PRODUIT -----------
| Last update : 15 Feb 2006                        |
|  Function : read the coordinates of a polygon    |
--------------------------------------------------*/

///////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "caliconfig.h"
#include "calierror.h"
#include "calimacros.h"
#include "read1Poly.h"
#include "util.h"

///////////////////////////////////////////////////////
/* Read a polygone
Input arguments:
fp: pointer on the input file
Output arguments:
ID: identificator of the polygone
nsom: number of vertices
lesx: x-coordinates
lesy: y-coordinates
Return:
0 when no error
otherwise, another value
*/
///////////////////////////////////////////////////////
// Format 1:
// npoly
// identificator, followed by the x-coordinates
// same identificator, followed by the y-coordinates
///////////////////////////////////////////////////////

int
read1Poly (FILE * fp, char *pdelim,
	   int &ID, int &nsom, real lesx[], real lesy[])
{
  char lu[MAX_LINE_POLY];
  char *p;
  int idim=0, iget=0, nsompred=0, idpred=0;
  real lucoord;
  char moi[] = "read1Poly";
/* To save the messages */
  char errmess[CHAR_MAX];



  for (idim = 0; idim < DIM; idim++)
    {
      iget = atoi (fgets (lu, MAX_LINE_POLY, fp));

      // Detect the empty lines and the end of file
      if ( strpbrk (lu, "0123456789") == NULL)
	{
	  snprintf (errmess, CHAR_MAX, "premature end of file\n");
          return (ecrmess (CALI_ERPOLY1, moi, errmess));
	}

      p = strtok (lu, pdelim);
      ID = atoi (p);

      if (idim == 0)
	idpred = ID;
      else
	{
	  if (ID != idpred)
	    {
	      snprintf (errmess, CHAR_MAX,
		       "premature end of file\npoly %d not finished\n",
		       idpred);
	      return (ecrmess (CALI_ERPOLY2, moi, errmess));
	    }			// end ( ID != idpred)
	}			// end else


      nsom = 0;

      while ((p = strtok (NULL, pdelim)) != NULL)
	{
	  lucoord = atof (p);
	  if (lucoord <= -real (INT_MAX))
	    {
	      snprintf (errmess, CHAR_MAX,
		       "polygon ident %d has too small coordinates (%g <= %d)\n",
		       ID, lucoord, INT_MAX);
	      return (ecrmess (CALI_ERPOLY8, moi, errmess));
	    }

	  //      if (lucoord < 0) {
//         snprintf (errmess, CHAR_MAX,
//                  "polygon ident %d has negative coordinates (%g)\n",
//                  ID, lucoord);
//         return (ecrmess(CALI_ERPOLY8, moi, errmess));
//       }

	  if (nsom > MAX_VERTICES)
	    {
	      snprintf (errmess, CHAR_MAX,
		       "polygon ident %d has %d vertices  (maximum MAX_VERTICES= %d)\n",
		       ID, nsom, MAX_VERTICES);
	      return (ecrmess (CALI_ERPOLY7, moi, errmess));
	    }

	  if (idim == 0)
	    lesx[nsom] = lucoord;
	  else
	    lesy[nsom] = lucoord;
	  nsom++;

	}			// end while

      if (idim == 0)
	nsompred = nsom;
      else
	{
	  if (nsompred != nsom)
	    {
	      snprintf (errmess, CHAR_MAX,
		       "polygon ident %d has %d x-vertices and %d y-vertices.\n",
		       ID, nsompred, nsom);
	      return (ecrmess (CALI_ERPOLY3, moi, errmess));
	    }			// end if ( nsompred != nsom)
	}			// end else
    }				// end idim

  return (OK);
}				// end read1Poly

///////////////////////////////////////////////////////
// Format 2:
// first line: npoly 
// second line: identificator, name, nsom
// third line: the x-coordinates
// fourth line: the y-coordinates
///////////////////////////////////////////////////////
int
read2Poly (FILE * fp, char *pdelim, int &ID, char *nom,
	   int &nsom, real lesx[], real lesy[])
{
  char lu[MAX_LINE_POLY];
  char *p;
  int idim, isom, iget;
  real lucoord;
  char moi[] = "read2Poly";
/* To save the messages */
  char errmess[CHAR_MAX];

  iget = atoi (fgets (lu, MAX_LINE_POLY, fp));
  // Detect the empty lines and the end of file
  // if (   (iget == EOF) 
  // doesn't work when empty or blank lines at the end of the file,
  // so, we search for numbers on the line
  if ((iget == 0) || (strpbrk (lu, "0123456789") == NULL))
    {
      snprintf (errmess, CHAR_MAX, "premature end of file\n");
      return (ecrmess (CALI_ERPOLY1, moi, errmess));
    }

  // Read the polygon number, its name, the number of its vertices 
  // Read by fgets, because the name can be several words
  p = strtok (lu, pdelim);
  if (p == NULL)
    {
      snprintf (errmess, CHAR_MAX, "premature end of file\n");
      return (ecrmess (CALI_ERPOLY1, moi, errmess));
    }
  ID = atoi (p);
  p = strtok (NULL, pdelim);

  if (p == NULL)
    {
      snprintf (errmess, CHAR_MAX, "Bad format or delimitor at line \n");
      return (ecrmess (CALI_ERPOLY9, moi, errmess));
    }
  strcpy (nom, p);


  p = strtok (NULL, pdelim);
  nsom = atoi (p);
  if (nsom > MAX_VERTICES)
    {
      snprintf (errmess, CHAR_MAX,
	       "polygon ident %d has %d vertices  (maximum MAX_VERTICES=%d)\n",
	       ID, nsom, MAX_VERTICES);
      return (ecrmess (CALI_ERPOLY7, moi, errmess));
    }


  //Read the coordinates:


  for (idim = 0; idim < DIM; idim++)
    {
      iget = atoi (fgets (lu, MAX_LINE_POLY, fp));

      // Detect the empty lines and the end of file
      // 21/11/16      if ((iget == 0) || (strpbrk (lu, "0123456789") == NULL))
      if ( strpbrk (lu, "0123456789") == NULL)
	{
	  snprintf (errmess, CHAR_MAX,
		   "premature end of file\npoly %d not finished\n", ID);
	  return (ecrmess (CALI_ERPOLY2, moi, errmess));
	}

      for (isom = 0; isom < nsom; isom++)
	{
	  if (isom == 0)
	    p = strtok (lu, pdelim);
	  else
	    p = strtok (NULL, pdelim);
	  if (p == NULL)
	    {
	      snprintf (errmess, CHAR_MAX,
		       "polygon ident %d should have %d vertices", ID, nsom);
	      return (ecrmess (CALI_ERPOLY4, moi, errmess));
	    }
	  lucoord = atof (p);
	  if (lucoord <= -real (INT_MAX))
	    {
	      snprintf (errmess, CHAR_MAX,
		       "polygon ident %d has too small coordinates (%g <= %d)\n",
		       ID, lucoord, INT_MAX);
	      return (ecrmess (CALI_ERPOLY8, moi, errmess));
	    }

	  if (idim == 0)
	    lesx[isom] = lucoord;
	  else
	    lesy[isom] = lucoord;

	}			// end isom

    }				// end idim

  return (OK);
}				// end read2Poly
