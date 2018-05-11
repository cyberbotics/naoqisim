ifeq ($(WEBOTS_HOME),)
$(error WEBOTS_HOME environment variable is not set, please set it before running this Makefile)
endif

default clean:
	@+echo "# naoqisim controller"
	@+make --silent -C controllers/naoqisim $(MAKECMDGOALS)
	@+echo "# challenge_solder controller "
	@+make --silent -C controllers/challenge_solver $(MAKECMDGOALS)
	@+echo "# nao_soccer library"
	@+make --silent -C libraries/nao_soccer $(MAKECMDGOALS)
	@+echo "# soccer_vision library"
	@+make --silent -C libraries/soccer_vision $(MAKECMDGOALS)
	@+echo "# nao_team_0 controller"
	@+make --silent -C controllers/nao_team_0 $(MAKECMDGOALS)
	@+echo "# robocup_striker controller"
	@+make --silent -C controllers/robocup_striker $(MAKECMDGOALS)
