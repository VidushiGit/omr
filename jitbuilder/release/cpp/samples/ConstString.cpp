/*******************************************************************************
 * Copyright IBM Corp. and others 2016
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] https://openjdk.org/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "ConstString.hpp"

static void printString(const char* val)
   {
   #define PRINTSTRING_LINE LINETOSTR(__LINE__)
   printf("%s", val);
   }

ConstStringMethod::ConstStringMethod(OMR::JitBuilder::TypeDictionary *d)
   : OMR::JitBuilder::MethodBuilder(d)
   {
   DefineLine(LINETOSTR(__LINE__));
   DefineFile(__FILE__);

   DefineName("testConstString");

   DefineReturnType(NoType);

   DefineFunction((const char* const)"printString", 
                  (const char* const)__FILE__,
                  (const char* const)PRINTSTRING_LINE,
                  (void *)&printString,
                  NoType,
                  1,
                  Address);
   }

bool
ConstStringMethod::buildIL()
   {
   OMR::JitBuilder::IlValue* value = ConstString("The test string is: Hello World!\n");
   Call("printString", 1,
      value);

   Return();

   return true;
   }


int
main(int argc, char *argv[])
   {
   printf("Step 1: initialize JIT\n");
   bool initialized = initializeJit();
   if (!initialized)
      {
      fprintf(stderr, "FAIL: could not initialize JIT\n");
      exit(-1);
      }

   printf("Step 2: define type dictionary\n");
   OMR::JitBuilder::TypeDictionary types;

   printf("Step 3: compile method builder\n");
   ConstStringMethod method(&types);
   void *entry;
   int32_t rc = compileMethodBuilder(&method, &entry);
   if (rc != 0)
      {
      fprintf(stderr,"FAIL: compilation error %d\n", rc);
      exit(-2);
      }

   printf("Step 4: invoke compiled code and verify results\n");
   ConstStringFunctionType *test = (ConstStringFunctionType *)entry; 
   test();

   printf ("Step 5: shutdown JIT\n");
   shutdownJit();

   printf("PASS\n");
   }
