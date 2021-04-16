# frozen_string_literal: true

require 'rack/utils'

helpers do
  def h(text)
    Rack::Utils.escape_html(text)
  end

  def ensure_user
    unless cookies[:userid]
      cookies[:back] = request.path_info
      redirect '/username'
    end
    cookies[:userid]
  end

  def ensure_board(boards)
    board = boards[params[:board]]
    redirect '/' unless board
    board
  end
end
