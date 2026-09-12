# LinuxTouch
#
# Two workflows:
#   make mock                 develop the shell on this workstation
#   make deploy               push shell + daemon to a board and restart
#
# The board's address is never stored in a tracked file. The first dev target
# asks for it and offers to write scripts/dev.env, which is gitignored.

SHELL := /bin/bash
MOCK_PORT ?= 8721

# `make deploy DEVICE=user@host` overrides scripts/dev.env for one invocation.
DEVFLAGS :=
ifdef DEVICE
DEVFLAGS += --device $(DEVICE)
endif
ifdef KEY
DEVFLAGS += --key $(KEY)
endif

.PHONY: help
help:
	@echo "LinuxTouch targets:"
	@echo "  make mock          serve the shell at 640x480 against a fake daemon"
	@echo "  make dev-env       configure which board to talk to"
	@echo "  make deploy        push everything to the board and restart it"
	@echo "  make deploy-shell  push only the web bundle (about a second)"
	@echo "  make overlay       build the GT911 device-tree overlay locally"
	@echo "  make check         syntax-check Python and shell scripts"
	@echo "  make test          run the unit tests"
	@echo "  make ssh           open a shell on the board"
	@echo "  make logs          follow touchd's journal on the board"

# ── development ──────────────────────────────────────────────────────

.PHONY: mock
mock:
	@echo "Serving the shell on http://127.0.0.1:$(MOCK_PORT)/?mock=1"
	@echo "Home = Start, End = Select, Esc = Back. Ctrl-C to stop."
	@echo
	@echo "Resize the browser window to exactly 640x480 — the shell has no"
	@echo "responsive layout, because the device has exactly one screen size."
	@cd shell && python3 -m http.server $(MOCK_PORT) --bind 127.0.0.1

.PHONY: dev-env
dev-env:
	@. scripts/dev-env.sh $(DEVFLAGS); \
	  echo "device $$DEVICE"; echo "key    $$KEY"; echo "port   $$PORT"

# ── deployment ───────────────────────────────────────────────────────

.PHONY: deploy
deploy:
	@scripts/deploy.sh $(DEVFLAGS)

.PHONY: deploy-shell
deploy-shell:
	@scripts/deploy.sh $(DEVFLAGS) --shell-only

.PHONY: ssh
ssh:
	@. scripts/dev-env.sh $(DEVFLAGS); ssh $$SSH_OPTS "$$DEVICE"

.PHONY: logs
logs:
	@. scripts/dev-env.sh $(DEVFLAGS); \
	  ssh $$SSH_OPTS "$$DEVICE" 'journalctl --user -u touchd -f --no-pager'

# ── build ────────────────────────────────────────────────────────────

.PHONY: overlay
overlay: overlays/linuxtouch-gt911.dtbo

overlays/linuxtouch-gt911.dtbo: overlays/linuxtouch-gt911-overlay.dts
	@command -v dtc >/dev/null || { echo "dtc not found: apt install device-tree-compiler" >&2; exit 1; }
	dtc -@ -I dts -O dtb -o $@ $<
	@echo "built $@"

# ── checks ───────────────────────────────────────────────────────────

.PHONY: check
check:
	@echo "== python =="
	@python3 -m compileall -q touchd/touchd && echo "ok"
	@echo "== shell scripts =="
	@for f in bin/* scripts/*.sh; do bash -n "$$f" || exit 1; done; echo "ok"
	@echo "== javascript =="
	@if command -v node >/dev/null; then \
	  for f in shell/js/*.js; do node --check "$$f" || exit 1; done; echo "ok"; \
	else echo "node not installed; skipped"; fi

.PHONY: test
test:
	@python3 -m pytest touchd/tests -q

.PHONY: clean
clean:
	rm -f overlays/*.dtbo
	find . -name __pycache__ -type d -prune -exec rm -rf {} +
