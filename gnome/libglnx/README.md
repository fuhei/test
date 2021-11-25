GNOME libglnx
==============

* [gitlab libglnx](https://gitlab.gnome.org/GNOME/libglnx)


有多个项目对 Linux 和 GLib 有严格的依赖性，例如 NetworkManager、ostree、flatpak 等。它们能够共享 Linux 特定的 API 是有意义的。
该模块还包含一些取自 systemd 的代码，它具有非常高质量的 LGPLv2+ 共享库代码，但大部分内部共享库是私有的，没有命名空间。
也可以将这个项目与 gnulib 进行比较；显着的区别在于，至少该模块的某些部分最终注定要包含在 GLib 中。


