#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************



PRJ=..$/..
PRJNAME=basegfx
TARGET=tuple

#UNOUCRRDB=$(SOLARBINDIR)$/applicat.rdb
#ENABLE_EXCEPTIONS=FALSE
#USE_DEFFILE=TRUE

# --- Settings ----------------------------------

.INCLUDE :  	settings.mk

# --- Files -------------------------------------

SLOFILES= \
				$(SLO)$/b2dtuple.obj		\
				$(SLO)$/b3dtuple.obj		\
				$(SLO)$/b2ituple.obj		\
				$(SLO)$/b3ituple.obj		\
				$(SLO)$/b2i64tuple.obj		\
				$(SLO)$/b3i64tuple.obj

# --- Targets ----------------------------------

.INCLUDE : target.mk
