module hanoi_mod
    integer :: move_count = 0
contains
    recursive subroutine tower_of_hanoi(n, from_peg, to_peg, aux_peg)
        integer, intent(in) :: n
        character, intent(in) :: from_peg, to_peg, aux_peg
        if (n == 1) then
            move_count = move_count + 1
            return
        end if
        call tower_of_hanoi(n - 1, from_peg, aux_peg, to_peg)
        move_count = move_count + 1
        call tower_of_hanoi(n - 1, aux_peg, to_peg, from_peg)
    end subroutine tower_of_hanoi
end module hanoi_mod

program main
end program main
