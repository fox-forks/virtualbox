# $Id$
## @file
# IPRT - SED script for generating a list of assembly files for make inclusion.
#

#
# Copyright (C) 2012-2023 Oracle and/or its affiliates.
#
# This file is part of VirtualBox base platform packages, as
# available from https://www.virtualbox.org.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, in version 3 of the
# License.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <https://www.gnu.org/licenses>.
#
# The contents of this file may alternatively be used under the terms
# of the Common Development and Distribution License Version 1.0
# (CDDL), a copy of it is provided in the "COPYING.CDDL" file included
# in the VirtualBox distribution, in which case the provisions of the
# CDDL are applicable instead of those of the GPL.
#
# You may elect to license modified versions of this file under the
# terms and conditions of either the GPL or the CDDL or both.
#
# SPDX-License-Identifier: GPL-3.0-only OR CDDL-1.0
#

# Header and footer.
1b header
$b footer

# Drop all lines from the start of the file until the SED: START marker.
1,/SED: START/d

# Drop all lines from the SED: END marker and till the end of the file.
/SED: END/,$d

# We are only interested in the SUPEXP_STK_BACK lines.
/^ *SUPEXP_STK_BACKF*(/!d
s/^ *SUPEXP_STK_BACKF*( *\([0-9][0-9]*\) *, *\([^)][^)]*\)),.*$/\/\/ ##### BEGINFILE \"StkBack_\2.asm\"\n%include "VBox\/SUPR0StackWrapper.mac"\nSUPR0StackWrapperGeneric \2, \1\n\/\/ ##### ENDFILE/
b end

:header
i\#
i\# Autogenerated. DO NOT EDIT!
i\#
d


:footer
d

:end
