#ident	"@(#)fingerd:$Name:  $:$Id: acconfig.h,v 1.4 1999/01/15 18:09:56 woods Exp $"

@TOP@

/* The name of the package. */
#define PACKAGE "fingerd"

/* The package release identifier. */
#define VERSION	"Pre-release"

/* path to the finger program */
#define FINGER_PATH		"/usr/bin/finger"

/* path to the motd file */
#define FINGERD_MOTD		"/usr/local/share/fingerd/motd"

/* path to the acl file */
#define FINGERD_ACL		"/usr/local/share/fingerd/access"

/* path to the users file */
#define FINGERD_USERS		"/usr/local/share/fingerd/users"
