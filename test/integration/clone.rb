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

Dir.mktmpdir do |dir1|
  git.init("--bare", dir1)

  Dir.mktmpdir do |dir2|
    git.__clone(dir1, dir2)

    Dir.chdir(dir2) do
      File.open("f1.txt", "w") { |f| f.write("Holla") }
      git.add("f1.txt")
      git.commit("-m 'Initial commit'")
      git.push("origin master:master")
    end
  end

  Dir.mktmpdir do |dir2|
    git.__clone("ssh://localhost:4711#{dir1}", dir2, :password => true)

    Dir.chdir(dir2) do
      oid = git.log("--pretty='format:%H'").strip
      bare_oid = Dir.chdir(dir1) { git.rev_parse("HEAD") }.strip

      assert(oid == bare_oid)
    end
  end
end
