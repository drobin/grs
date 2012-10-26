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

module Grs
  module Assertion
    def assert(test, failure_message = nil)
      unless test
        raise failure_message || test.to_s
      end
    end

    def assert_ref(expected_ref, actual_ref)
      expected_oid = expected_ref[0].strip
      actual_oid = actual_ref[0].strip

      assert(expected_oid == actual_oid,
             "Unexpected object-id. " +
             "Expected #{expected_oid} but is #{actual_oid}")

      expected_ref_name = expected_ref[1].strip
      actual_ref_name = actual_ref[1].strip

      assert(expected_ref_name == actual_ref_name,
             "Unexpected reference-name. " +
             "Expected #{expected_ref_name} but is #{actual_ref_name}")
    end
  end
end

class Object
  include Grs::Assertion
end
