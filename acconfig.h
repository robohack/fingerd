#ident	"@(#)fingerd:$Name:  $:$Id: acconfig.h,v 1.3 1997/09/15 00:39:04 woods Exp $"

@TOP@

/* The name of the package. */
#define PACKAGE "fingerd"

/* The package release identifier. */
#define VERSION	"Pre-release"

/* path to the finger program */
#define FINGER_PATH		"/usr/bin/finger"

/* path to the motd file */
#define FINGER_MOTD		"/usr/local/share/fingerd/motd"

/* path to the acl file */
#define FINGER_ACL		"/usr/local/share/fingerd/access"

/* path to the users file */
#define FINGER_USERS		"/usr/local/share/fingerd/users"
