#!/usr/bin/env ruby

################################################################################
#
# This file is part of grs.
#
# grs is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# grs is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with grs.  If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

require "tempfile"
require_relative "grs/assert"
require_relative "grs/git_wrapper"

git = Grs::Git.new

bare_repo = Dir.mktmpdir
clone_repo = Dir.mktmpdir

begin
  git.init("--bare", bare_repo)

  git.__clone(bare_repo, clone_repo)

  Dir.chdir(clone_repo) do
    File.open("f1.txt", "w") { |f| f.write("Holla") }
    git.add("f1.txt")
    git.commit("-m '1st commit'")

    git.tag("t1", "HEAD")

    git.push("origin master:master")
    git.push("origin t1")

    git.remote("set-url origin ssh://localhost:4711#{bare_repo}")
    refs = git.ls_remote(:password => true).split("\n")

    head_oid = Dir.chdir(bare_repo) { git.rev_parse("HEAD") }

    assert(refs.count == 3, "Three refs expected")
    assert_ref([head_oid, "HEAD"], refs[0].split)
    assert_ref([head_oid, "refs/heads/master"], refs[1].split)
    assert_ref([head_oid, "refs/tags/t1"], refs[2].split)
  end
ensure
  FileUtils.remove_entry_secure(bare_repo)
  FileUtils.remove_entry_secure(clone_repo)
end
