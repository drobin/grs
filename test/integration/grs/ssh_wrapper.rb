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

require 'pty'

module Grs
  class Ssh
    def self.pass(command)
      password = ENV["USER"]
      r, w = IO.pipe

      PTY.spawn(command, :out => w) do |out, input, pid|
        buffer = ""

        while c = out.getc
          buffer << c.chr
          if buffer =~ /password:/
            input.write "#{password}\r"
            break
          end
        end

        Process.wait(pid)
      end

      buffer = ""
      loop do
        begin
          buffer << r.read_nonblock(16)
        rescue
          break
        end
      end

      r.close
      w.close

      return buffer
    end
  end
end
