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
grs_repo = Dir.mktmpdir

begin
  git.init("--bare", bare_repo)

  git.__clone(bare_repo, clone_repo)

  Dir.chdir(clone_repo) do
    File.open("f1.txt", "w") { |f| f.write("Holla") }
    git.add("f1.txt")
    git.commit("-m '1st commit'")

    File.open("f2.txt", "w") { |f| f.write("Hallo") }
    git.add("f2.txt")
    git.commit("-m '2nd commit'")

    git.push("origin master:master")
  end

  git.__clone("ssh://localhost:4711#{bare_repo}", grs_repo, :password => true)

  Dir.chdir(clone_repo) do
    File.open("f1.txt", "w") { |f| f.write("Holla die Waldfee") }
    git.add("f1.txt")
    git.commit("-m '3rd commit'")

    git.rm("f2.txt")
    git.commit("-m '4th commit'")

    git.push("origin master:master")
  end

  Dir.chdir(grs_repo) do
    git.pull(:password => true)

    log = git.log("--pretty='format:%H'").strip.split
    bare_log = Dir.chdir(bare_repo) { git.log("--pretty='format:%H'") }.strip.split

    assert(log.size == 4)
    assert(bare_log.size == 4)
    assert(log[0] == bare_log[0])
    assert(log[1] == bare_log[1])
    assert(log[2] == bare_log[2])
    assert(log[3] == bare_log[3])
  end
ensure
  FileUtils.remove_entry_secure(bare_repo)
  FileUtils.remove_entry_secure(clone_repo)
  FileUtils.remove_entry_secure(grs_repo)
end
