# Task 10

## This is Task 10 of the Eudyptula Challenge

Yeah, you conquered the sysfs monster, great job!

Now you know to never want to mess with a kobject again, right?  Trust
me, there are easier ways to create sysfs files, and we will get into
that for a future task, but for now, let's make it a bit more simple
after all of that coding.

For this task, go back to the linux-next tree you used for task 07.
Update it, and then do the following:
  - Create a patch that fixes one coding style problem in any of the
    files in drivers/staging/
  - Make sure the patch is correct by running it through
    scripts/checkpatch.pl
  - Submit the code to the maintainer of the driver/subsystem, finding
    the proper name and mailing lists to send it to by running the tool,
    scripts/get_maintainer.pl on your patch.
  - Send a web link back to me of your patch on the public mailing list
    archive (don't cc: me on the patch, that will only confuse me and
    everyone in the kernel development community.)
  - If you want to mention the Eudyptula Challenge as the reason for
    writing the patch, feel free to do so in the body of the patch, but
    it's not necessary at all.

Hopefully this patch will be accepted into the kernel tree, and all of a
sudden, you are an "official" kernel developer!

Don't worry, there's plenty more tasks coming, but a little breather
every now and again for something simple is always nice to have.

## Solution

### staging patch

https://lore.kernel.org/linux-staging/20220626020002.1577101-1-me@davidreaver.com/T/

```patch
From 9b2d2d500a4ca8f730326a3d841543913d7ecb75 Mon Sep 17 00:00:00 2001
From: David Reaver <me@davidreaver.com>
Date: Sat, 25 Jun 2022 13:32:30 -0700
Subject: [PATCH] staging: fbtft: fix alignment should match open parenthesis

Fix alignment of this line of code with the previous parenthesis, as
suggested by checkpatch.pl:

  $ ./scripts/checkpatch.pl -f drivers/staging/fbtft/fb_tinylcd.c
  CHECK: Alignment should match open parenthesis
  #41: FILE: drivers/staging/fbtft/fb_tinylcd.c:41:
  +       write_reg(par, 0xE0, 0x00, 0x35, 0x33, 0x00, 0x00, 0x00,
  +                      0x00, 0x35, 0x33, 0x00, 0x00, 0x00);

Signed-off-by: David Reaver <me@davidreaver.com>
---
 drivers/staging/fbtft/fb_tinylcd.c | 2 +-
 1 file changed, 1 insertion(+), 1 deletion(-)

diff --git a/drivers/staging/fbtft/fb_tinylcd.c b/drivers/staging/fbtft/fb_tinylcd.c
index 9469248f2c50..60cda57bcb33 100644
--- a/drivers/staging/fbtft/fb_tinylcd.c
+++ b/drivers/staging/fbtft/fb_tinylcd.c
@@ -38,7 +38,7 @@ static int init_display(struct fbtft_par *par)
 	write_reg(par, 0xE5, 0x00);
 	write_reg(par, 0xF0, 0x36, 0xA5, 0x53);
 	write_reg(par, 0xE0, 0x00, 0x35, 0x33, 0x00, 0x00, 0x00,
-		       0x00, 0x35, 0x33, 0x00, 0x00, 0x00);
+		  0x00, 0x35, 0x33, 0x00, 0x00, 0x00);
 	write_reg(par, MIPI_DCS_SET_PIXEL_FORMAT, 0x55);
 	write_reg(par, MIPI_DCS_EXIT_SLEEP_MODE);
 	udelay(250);
```

### get_feat.pl perl patch

https://lore.kernel.org/lkml/20220625211548.1200198-1-me@davidreaver.com/T/

```patch
From 1998041c53bc9935c6841d269d8130fb85082fba Mon Sep 17 00:00:00 2001
From: David Reaver <me@davidreaver.com>
Date: Sat, 25 Jun 2022 13:41:03 -0700
Subject: [PATCH] scripts: get_feat.pl: use /usr/bin/env to find perl

I tried running `make pdfdocs` on NixOS, but it failed because
get_feat.pl uses a shebang line with /usr/bin/perl, and that file path
doesn't exist on NixOS. Using the more portable /usr/bin/env perl fixes
the problem.

Signed-off-by: David Reaver <me@davidreaver.com>
---
 scripts/get_feat.pl | 2 +-
 1 file changed, 1 insertion(+), 1 deletion(-)

diff --git a/scripts/get_feat.pl b/scripts/get_feat.pl
index 76cfb96b59b6..5c5397eeb237 100755
--- a/scripts/get_feat.pl
+++ b/scripts/get_feat.pl
@@ -1,4 +1,4 @@
-#!/usr/bin/perl
+#!/usr/bin/env perl
 # SPDX-License-Identifier: GPL-2.0

 use strict;
```
