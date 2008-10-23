#!/usr/bin/perl -w

eval 'exec perl -S $0 ${1+"@"}' if $running_under_some_shell;

use Gtk '-init';
my $window = new Gtk::Window;
$window->set_title("MIB Browsewr");
$window->set_policy(0,1,1);
$window->signal_connect('delete_event', sub {Gtk->main_quit; return 1;});
my $frame = new Gtk::Frame("My Frame");
$frame->set_shadow_type('etched-in');
my $entry = new Gtk::Entry;
$entry->set_text("Some Text");
$entry->set_visibility(1);
$entry->set_editable(1);

my $button = new Gtk::Button("Quit");
$button->signal_connect("clicked", sub {Gtk->main_quit});
my $table = new Gtk::Table(2,1,1);
$table->attach_defaults($entry,1,0,1,0);
$table->attach_defaults($button,1,0,0,1);
$table->add($entry);
$table->add($button);
$frame->add($table);
$window->add($frame);
$window->show_all;
Gtk->main;

