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

require 'open3'
require 'pty'

module Grs
  class Git
    def self.sh(args)
      Open3.popen3("git #{args}") do |stdin, stdout, stderr, wait_thr|
        out = stdout.read
        err = stderr.read
        exit_status = wait_thr.value

        unless exit_status.success?
          $stderr.puts err
          raise "Failed to exec 'git #{args}'"
        end
        out
      end
    end

    def self.pass(args)
      password = ENV["USER"]
      r, w = IO.pipe

      PTY.spawn("git #{args}", :out => w) do |out, input, pid|
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

    def method_missing(method, *args)
      options = Git.pop_options(args)

      git_method = method.to_s.sub(/^_{1,}/, "").sub(/_/, "-")
      git_args = [git_method, args].flatten.join(" ")

      if options[:password]
        Git.pass(git_args)
      else
        Git.sh(git_args)
      end
    end

    private

    def self.pop_options(args)
      options = args.last
      options.is_a?(Hash) ? args.pop : {}
    end
  end
end
