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
  git.__clone(bare_repo, clone_repo, :password => true)

  Dir.chdir(clone_repo) do
    File.open("f1.txt", "w") { |f| f.write("Holla") }
    git.add("f1.txt")
    git.commit("-m '1st commit'")

    File.open("f2.txt", "w") { |f| f.write("Hallo") }
    git.add("f2.txt")
    git.commit("-m '2nd commit'")

    git.push("origin master:master")
  end

  bare_log = Dir.chdir(bare_repo) { git.log("--pretty='format:%H'") }.strip.split
  clone_log = Dir.chdir(clone_repo) { git.log("--pretty='format:%H'") }.strip.split
  assert(bare_log.count == clone_log.count)
  assert(bare_log[0] == clone_log[0])
  assert(bare_log[1] == clone_log[1])

  Dir.chdir(clone_repo) do
    git.remote("set-url origin ssh://localhost:4711#{bare_repo}")
    git.push(:password => true)
  end

  clone_log = Dir.chdir(clone_repo) { git.log("--pretty='format:%H'") }.strip.split
  assert(bare_log.count == clone_log.count)
  assert(bare_log[0] == clone_log[0])
  assert(bare_log[1] == clone_log[1])
ensure
  FileUtils.remove_entry_secure(bare_repo)
  FileUtils.remove_entry_secure(clone_repo)
end
