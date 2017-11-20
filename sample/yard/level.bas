REM Yet Another RPG Dungeon is a text based game.
REM It's aimed to be a comprehensive example and a tutorial of MY-BASIC.
REM For more information, see https://github.com/paladin-t/my_basic/

import "goal.bas"
import "map.bas"
import "monster.bas"
import "npc.bas"

class game_status
	var invalid = 0
	var playing = 1
	var win = 2
	var lose = 3
endclass

class level
	var status = game_status.invalid
	var roads = new(map)
	var role = nil

	def set_node(x, y, ...)
		n = new(map_node)
		roads.set_node(x, y, n)
		n.set_valid_dirs(...)

		return n
	enddef

	def get_node(x, y)
		return roads.get_node(x, y)
	enddef

	def create()
		roads.set_size(3, 3)

		n = set_node(0, 0, dirs.right_dir)

		n = set_node(1, 0, dirs.left_dir, dirs.right_dir, dirs.up_dir)

		n = set_node(2, 0, dirs.left_dir, dirs.up_dir)

		m = new(monster)
		m.name = "Skeleton"
		m.init(7, 1, 1)
		m.dead_handler = lambda (_)
		(
			print "- " + _.name + " dead";
		)
		n.add_entity(m)

		n = set_node(0, 1, dirs.right_dir)

		n = set_node(1, 1, dirs.left_dir, dirs.up_dir, dirs.down_dir)

		m = new(monster)
		m.name = "Slime"
		m.init(5, 1, 1)
		m.dead_handler = lambda (_)
		(
			print "- " + _.name + " dead";
		)
		n.add_entity(m)

		n = set_node(2, 1, dirs.up_dir, dirs.down_dir)

		n = set_node(0, 2, dirs.right_dir)

		g = new(goal)
		g.name = "Goal"
		n.add_entity(g)

		n = set_node(1, 2, dirs.left_dir, dirs.right_dir, dirs.down_dir)

		m = new(monster)
		m.name = "Dragon"
		m.init(10, 5, 1)
		m.dead_handler = lambda (_)
		(
			print "- " + _.name + " dead";
		)
		n.add_entity(m)

		n = set_node(2, 2, dirs.left_dir, dirs.down_dir)

		c = new(npc)
		c.name = "Therapist"
		c.talk_handler = lambda (_)
		(
			print "- " + _.name + ": gives you 23 HP";
			_.alive = false
			role.hp = role.hp + 23
		)
		n.add_entity(c)
	enddef

	def start(r)
		status = game_status.playing
		role = r
		role.pos.x = 0
		role.pos.y = 0
	enddef

	def format()
		if status = game_status.win then
			print "You Win";
		elseif status = game_status.lose then
			print "Game Over";

			return
		elseif status = game_status.playing then
			print "<" + str(role.pos.x) + ", " + str(role.pos.y) + ">";

			n = roads.get_node(role.pos.x, role.pos.y)

			contains_goal = false
			gl = n.get_entities(goal)
			it = iterator(gl)
			if move_next(it) then
				contains_goal = true
				ent = get(it)
				print "  ", ent;
				it = nil
			endif

			contains_npc = false
			np = n.get_entities(npc)
			it = iterator(np)
			if move_next(it) then
				contains_npc = true
				ent = get(it)
				print "  ", ent;
				it = nil
			endif

			contains_monster = false
			ms = n.get_entities(monster)
			it = iterator(ms)
			if move_next(it) then
				contains_monster = true
				ent = get(it)
				print "  ", ent;
				it = nil
			endif

			print "[Operations]";

			if contains_goal then
				print "  G: Accomplish goal";
			endif

			if contains_npc then
				print "  T: Talk to " + ent.name;
			endif

			if contains_monster then
				print "  A: Attack " + ent.name;
			else
				if bit_and(n.valid_dirs, dirs.left_dir) then
					print "  W: Go west";
				endif

				if bit_and(n.valid_dirs, dirs.right_dir) then
					print "  E: Go east";
				endif

				if bit_and(n.valid_dirs, dirs.up_dir) then
					print "  N: Go north";
				endif

				if bit_and(n.valid_dirs, dirs.down_dir) then
					print "  S: Go south";
				endif
			endif
		endif

		print "  Q: Quit game";
	enddef

	def update(i)
		if i = "" then
			return false
		endif

		ipt = to_lower_case(i)

		if ipt = "q" then print "Bye."; : end

		if status <> game_status.playing then
			return false
		endif

		n = roads.get_node(role.pos.x, role.pos.y)

		gl = n.get_entities(goal)
		it = iterator(gl)
		if ipt = "g" and move_next(it) then
			ent = get(it)
			it = nil
			status = game_status.win

			return true
		endif

		np = n.get_entities(npc)
		it = iterator(np)
		if ipt = "t" and move_next(it) then
			ent = get(it)
			it = nil
			ent.talk(role)

			return true
		endif

		contains_monster = false
		ms = n.get_entities(monster)
		it = iterator(ms)
		if move_next(it) then
			contains_monster = true

			if ipt = "a" then
				ent = get(it)
				it = nil
				ent.hurt_by(role)
				role.hurt_by(ent)

				return true
			endif
		endif

		if not contains_monster then
			if ipt = "w" and bit_and(n.valid_dirs, dirs.left_dir) then
				role.pos.x = role.pos.x - 1

				return true
			endif

			if ipt = "e" and bit_and(n.valid_dirs, dirs.right_dir) then
				role.pos.x = role.pos.x + 1

				return true
			endif

			if ipt = "n" and bit_and(n.valid_dirs, dirs.up_dir) then
				role.pos.y = role.pos.y + 1

				return true
			endif

			if ipt = "s" and bit_and(n.valid_dirs, dirs.down_dir) then
				role.pos.y = role.pos.y - 1

				return true
			endif
		endif

		print "Invalid input.";

		return false
	enddef
endclass
