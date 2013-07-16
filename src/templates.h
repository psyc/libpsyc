/*
  This file is part of libpsyc.
  Copyright (C) 2011,2012 Carlo v. Loesch, Gabor X Toth, Mathias L. Baumann,
  and other contributing authors.

  libpsyc is free software: you can redistribute it and/or modify it under the
  terms of the GNU Affero General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version. As a special exception, libpsyc is distributed with additional
  permissions to link libpsyc libraries with non-AGPL works.

  libpsyc is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
  details.

  You should have received a copy of the GNU Affero General Public License and
  the linking exception along with libpsyc in a COPYING file.
*/

#define _ PSYC_C2STRI

._notice_alias_add =
     _("[_nick] is now an alias for [_uniform]"),
._notice_alias_remove =
     _("[_nick] is no longer an alias for [_uniform]"),
._notice_alias_change =
     _("[_nick_old] is now known as [_nick_new]"),

._failure_alias_unavailable =
     _("[_nick] is already an alias for [_uniform]"),
._failure_alias_nonexistant =
     _("[_nick] is not an alias"),

._request_context_enter =
     _("[_source] asks for your permission to enter [_context]"),
._echo_context_enter =
     _("You enter [_source]" ),
._echo_context_leave =
     _("You leave [_source]"),

._notice_context_enter =
     _("[_source] enters [_context]"),
._notice_context_leave =
     _("[_source] leaves [_context]"),

._echo_hello =
     _("Received hello of psyc://[_key_public]:g/"),
._status_hello =
     _("Your hello is [_hello]"),
