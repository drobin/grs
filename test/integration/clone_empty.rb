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
grs_repo = Dir.mktmpdir

begin
  git.init("--bare", bare_repo)
  git.__clone("ssh://localhost:4711#{bare_repo}", grs_repo, :password => true)

  objects = Dir.chdir(grs_repo) do
    Dir.glob(".git/objects/**").reject{ |s| File.directory?(s) }
  end

  assert(objects.empty?)
ensure
  FileUtils.remove_entry_secure(bare_repo)
  FileUtils.remove_entry_secure(grs_repo)
end
